#include "EdgeBasedDuneDetector.h"
#include "DuneSegmentLinker.h"
#include "ConnectedComponentsExtractor.h"

#include <fstream>

namespace dune
{
EdgeBasedDuneDetector::EdgeBasedDuneDetector()
{
	ImageProcess = new EdgeDetectorImageProcessor();
}

EdgeBasedDuneDetector::EdgeBasedDuneDetector(EdgeDetectorImageProcessor* imgproc, const EdgeBasedDetectorParameters &params)
{
	ImageProcess = imgproc;
	Parameters = params;
}

EdgeBasedDuneDetector::~EdgeBasedDuneDetector()
{

}

std::vector<DuneSegment> EdgeBasedDuneDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	//cv::imshow("processedImage", processedImage);
	//cv::waitKey(0);

	cv::Mat crestlineImg = FilterByDominantOrientation(processedImage);

	std::vector<DuneSegment> duneSegs;
	duneSegs = GetDuneSegmentContours(crestlineImg);

	if (Parameters.ApplyLinking)
	{
		std::vector<DuneSegment> linkedSegs = LinkDuneSegments(duneSegs);
		return linkedSegs;
	}

	duneSegs = FilterSegmentsByMagnitude(duneSegs);

	return duneSegs;
}

double EdgeBasedDuneDetector::GetDominantOrientation(const cv::Mat &inputImg, const cv::Mat &edges)
{

	return 0.0;
}

cv::Mat EdgeBasedDuneDetector::FilterByDominantOrientation(const cv::Mat &edges)
{
	EdgeDetectorImageProcessor* edgeImgProc = dynamic_cast<EdgeDetectorImageProcessor*>(ImageProcess);
	double domOrientation = edgeImgProc->FindDominantOrientation(DominantOrientationMethod::HOG, Parameters.HistogramBins);
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
				if (diff < Parameters.AngleTolerance)
				{
					filtered.at<uchar>(y, x) = 255;
				}
			}
		}
	}

	return filtered;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::GetDuneSegmentContours(const cv::Mat &img)
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

	double threshold = meanM[0] + Parameters.R * stdDevM[0];

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

std::vector<DuneSegment> EdgeBasedDuneDetector::LinkDuneSegments(const std::vector<DuneSegment> &unlinked)
{
	SegmentLinkParameters params;
	params.DistanceThreshold = Parameters.LinkDistance;
	params.Method = EndPointToEndPoint;
	params.Type = Linear;
	DuneSegmentLinker linker(params);

	std::vector<DuneSegment> linked = linker.GetLinkedSegments(unlinked);

	return linked;
}

}