#include "EdgeBasedDuneDetector.h"
#include "EndpointSegmentLinker.h"
#include "LineSegmentLinker.h"
#include "ConnectedComponentsExtractor.h"

#include <fstream>

namespace dune
{
EdgeBasedDuneDetector::EdgeBasedDuneDetector()
{
	ImageProcess = new EdgeDetectorImageProcessor();
}

EdgeBasedDuneDetector::EdgeBasedDuneDetector(EdgeDetectorImageProcessor* imgproc, EdgeBasedDetectorParameters *params)
{
	ImageProcess = imgproc;
	Parameters = params;
}

EdgeBasedDuneDetector::~EdgeBasedDuneDetector()
{

}

void EdgeBasedDuneDetector::SetParameters(BaseDetectorParameters *params)
{
	Parameters = static_cast<EdgeBasedDetectorParameters*>(params);
	ImageProcess->SetParameters(Parameters->ImageProcessParameters);
}

std::vector<DuneSegment> EdgeBasedDuneDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	std::vector<DuneSegment> duneSegs;
	duneSegs = GetContourSegments(processedImage);

	if (Parameters->ApplyLinking)
	{
		
		std::vector<DuneSegment> linkedSegs = LinkDuneSegments(duneSegs);
		return linkedSegs;
	}

	duneSegs = FilterSegmentsByIntensityValues(duneSegs, img);

	return duneSegs;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::GetContourSegments(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	std::vector<DuneSegment> output;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		DuneSegment segment;
		std::vector<DuneSegmentData> data;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			/*double grad = edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(contours[i][j])[GRADIENT_MAT_MAGNITUDE_INDEX];
			if (grad < thresh)
			{
				data.push_back(DuneSegmentData(contours[i][j], 0));
			}*/
			data.push_back(DuneSegmentData(contours[i][j], 0));
		}
		segment.SetSegmentData(data);
		output.push_back(segment);
	}

	return output;

}

void EdgeBasedDuneDetector::ComputeSegmentLines(std::vector<DuneSegment> &segments)
{
	for (size_t i = 0; i < segments.size(); ++i)
	{
		segments[i].ComputeFeatures();
	}
}


cv::Mat EdgeBasedDuneDetector::FilterByDominantOrientation(const cv::Mat &edges)
{
	EdgeDetectorImageProcessor* edgeImgProc = dynamic_cast<EdgeDetectorImageProcessor*>(ImageProcess);
	double domOrientation = edgeImgProc->FindDominantOrientation(DominantOrientationMethod::HOG, Parameters->HistogramBins);
	//double domOrientation = edgeImgProc->FindDominantOrientation(DominantOrientationMethod::K_MEANS, 2);
	std::cout << domOrientation << std::endl;
	cv::Mat filtered = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
	for (int x = 0; x < edges.cols; ++x)
	{
		for (int y = 0; y < edges.rows; ++y)
		{
			if (edges.at<uchar>(y, x))
			{
				double dir = edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(y,x)[GRADIENT_MAT_DIRECTION_INDEX];

				double diff = std::fabs(dir - domOrientation);
				if (diff < Parameters->AngleTolerance)
				{
					filtered.at<uchar>(y, x) = 255;
				}
			}
		}
	}

	return filtered;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::GetDuneSegments(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;

	imgproc::ConnectedComponentsExtractor cce;
	contours = cce.GetCC(img);

	std::vector<DuneSegment> duneSegs;

	for (size_t i = 0; i < contours.size(); ++i)
	{
		std::vector<DuneSegmentData> segData;
		DuneSegment s;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			segData.push_back(DuneSegmentData(contours[i][j], 0));
		}
		s.SetSegmentData(segData);
		duneSegs.push_back(s);
	}

	return duneSegs;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::FilterSegmentsByMagnitude(const std::vector<DuneSegment> &input)
{
	EdgeDetectorImageProcessor* edgeImgProc = dynamic_cast<EdgeDetectorImageProcessor*>(ImageProcess);
	//double meanM = edgeImgProc->BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
	//double stdDevM = edgeImgProc->BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];

	std::vector<double> magnitudes;
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<DuneSegmentData> pts = input[i].GetSegmentData();
		for (size_t j = 0; j < pts.size(); ++j)
		{
			magnitudes.push_back(edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j].Position)[GRADIENT_MAT_MAGNITUDE_INDEX]);
		}
	}

	cv::Scalar meanM, stdDevM;
	cv::meanStdDev(magnitudes, meanM, stdDevM);

	double threshold = meanM[0] + Parameters->R * stdDevM[0];

	std::vector<DuneSegment> output;
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<DuneSegmentData> pts = input[i].GetSegmentData();
		double u = 0.0;
		for (size_t j = 0; j < pts.size(); ++j)
		{
			u += edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j].Position)[GRADIENT_MAT_MAGNITUDE_INDEX] / 1000.0;
		}
		u /= (double)pts.size() / 1000.0;
		if (u > threshold)
		{
			output.push_back(input[i]);
		}
	}

	return output;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::FilterSegmentsByLineOrientation(const std::vector<DuneSegment> &input)
{
	cv::Vec2d avgVec(0,0);
	for (size_t i = 0; i < input.size(); ++i)
	{
		cv::Vec4d line = input[i].GetLine();
		double len = input[i].GetLength();

		avgVec[0] += line[0] * len;
		avgVec[1] += line[1] * len;
	}

	double avgOrientation = std::atan2(avgVec[1], avgVec[0]);

	std::vector<DuneSegment> output;
	double thresh = 3.1416 / 4.0;
	for (size_t i = 0; i < input.size(); ++i)
	{
		cv::Vec4d line = input[i].GetLine();
		double orientation = std::atan2(line[1], line[0]);

		double low = fabs(orientation - avgOrientation);
		double high = fabs(orientation - (avgOrientation + 3.1416));
		if (low < thresh ||
			high < thresh)
		{
			output.push_back(input[i]);
		}
		else
		{
			double test = 0;
		}

	}

	return output;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::FilterSegmentsByIntensityValues(const std::vector<DuneSegment> &input, const cv::Mat &img)
{
	cv::Mat eqImg;
	cv::equalizeHist(img, eqImg);
	cv::Scalar mean, stddev;
	cv::meanStdDev(eqImg, mean, stddev);

	double threshold = mean[0] + Parameters->R * stddev[0];

	std::vector<DuneSegment> output;
	for (size_t i = 0; i < input.size(); ++i)
	{
		if (input[i].GetLength() > Parameters->MinSegmentLength)
		{
			std::vector<DuneSegment> split = SplitSegmentByIntensity(input[i], eqImg, threshold, Parameters->MinSegmentLength);
			for (size_t j = 0; j < split.size(); ++j)
			{
				output.push_back(split[j]);
			}
		}
	}

	return output;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::SplitSegmentByIntensity(const DuneSegment &input, const cv::Mat &img, double threshold, int neighbors)
{
	std::vector<DuneSegmentData> data = input.GetSegmentData();
	std::vector<bool> labels;

	for (int i = 0; i < data.size(); ++i)
	{
		int count = 0;
		for (int j = 0; j < neighbors; ++j)
		{
			int k = i - neighbors / 2 + j;
			if (k < 0)
				k = data.size() + k;
			else if (k >= (int)data.size())
				k = j - 1;

			if (img.at<uchar>(data[k].Position) > threshold)
			{
				count++;
			}
		}

		if ((double)count / (double)neighbors > 0.5)
		{
			labels.push_back(true);
		}
		else
		{
			labels.push_back(false);
		}
	}

	std::vector<DuneSegmentData> segment;
	std::vector<DuneSegment> output;

	int start = 0;
	while (labels[start] && start < labels.size()-1)
		start++;

	int i = start+1;
	if (i >= labels.size())
		i = 0;

	while (i != start)
	{
		while (!labels[i] && i != start)
		{
			i++;

			if (i >= labels.size())
				i = 0;	
		}

		while (labels[i] && i != start)
		{
			segment.push_back(data[i]);
			i++;
			if (i >= labels.size())
				i = 0;
		}
		if (segment.size() > Parameters->MinSegmentLength)
		{
			output.push_back(DuneSegment(segment));
		}
		segment.clear();
	}

	//wrap around for the contour
	return output;
}

std::vector<double> EdgeBasedDuneDetector::ComputeLineOrientationHistogram(std::vector<DuneSegment> &input)
{
	int dBins = 8;
	std::vector<double> histogram(dBins);
	std::vector<int> binIndex(input.size());
	for (size_t i = 0; i < histogram.size(); ++i)
	{
		histogram[i] = 0.0;
	}

	double increments = 3.1416 / (double)dBins;

	for (size_t i = 0; i < input.size(); ++i)
	{
		input[i].ComputeFeatures();
		cv::Vec4d line = input[i].GetLine();

		double orientation = std::atan2(line[1], line[0]);
		if (orientation < 0)
		{
			orientation += 3.1416;
		}


		int bin = std::ceil((orientation / increments) - 0.5);
		if (bin >= dBins)
			bin = 0;

		histogram[bin] += input[i].GetLength();
		binIndex[i] = bin;
	}

	//std::ofstream file("OrientationHistogram.txt");
	double max = 0.0;
	int maxBin = 0;
	for (size_t i = 0; i < histogram.size(); ++i)
	{
		//file << increments*(double)i << '\t' << histogram[i] << std::endl;
		if (histogram[i] > max)
		{
			max = histogram[i];
			maxBin = i;
		}
	}
	//file.close();

	std::vector<DuneSegment> filtered;
	for (size_t i = 0; i < input.size(); ++i)
	{
		if (binIndex[i] == maxBin)
		{
			filtered.push_back(input[i]);
		}
	}

	input = filtered;

	return histogram;
}




std::vector<DuneSegment> EdgeBasedDuneDetector::LinkDuneSegments(const std::vector<DuneSegment> &unlinked)
{
	//SegmentLinkParameters params;
	//params.DistanceThreshold = Parameters.LinkDistance;
	//params.Method = EndPointToEndPoint;
	//params.Type = Linear;
	//EndpointSegmentLinker linker(params);

	LineSegmentLinker linker;

	std::vector<DuneSegment> linked = linker.GetLinkedSegments(unlinked);

	return linked;
}

}