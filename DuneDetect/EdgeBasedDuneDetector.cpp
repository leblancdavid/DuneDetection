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

	cv::Mat crestlineImg = FilterByDominantOrientation(processedImage);

	std::vector<DuneSegment> duneSegs;
	duneSegs = GetDuneSegmentContours(crestlineImg);

	if (Parameters.ApplyLinking)
	{
		std::vector<DuneSegment> linkedSegs = LinkDuneSegments(duneSegs);
		duneSegs = FilterSegmentsByMagnitude(duneSegs);
		return linkedSegs;
	}
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

	double meanM = 0.0;
	double count = 0.0;
	double stdDevM = 0.0;
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<cv::Point> pts = input[i].GetEndPoints();
		for (size_t j = 0; j < pts.size(); ++j)
		{
			meanM += edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j])[GRADIENT_MAT_MAGNITUDE_INDEX] / 1000.0;
			count++;
		}
	}
	meanM /= count / 1000.0;

	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<cv::Point> pts = input[i].GetEndPoints();
		for (size_t j = 0; j < pts.size(); ++j)
		{
			stdDevM += fabs(edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j])[GRADIENT_MAT_MAGNITUDE_INDEX] - meanM) / 1000.0;
		}
	}
	stdDevM /= count / 1000.0;

	double threshold = meanM + Parameters.R * stdDevM;

	std::vector<DuneSegment> output;
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<cv::Point> pts = input[i].GetEndPoints();
		double u = 0.0;
		for (size_t j = 0; j < pts.size(); ++j)
		{
			u += edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j])[GRADIENT_MAT_MAGNITUDE_INDEX] / 1000.0;
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