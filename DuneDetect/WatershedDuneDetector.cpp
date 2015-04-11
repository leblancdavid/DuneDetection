#include "WatershedDuneDetector.h"

namespace dune
{

WatershedDuneDetector::WatershedDuneDetector()
{
	ImageProcess = new WatershedImageProcessor();
}

WatershedDuneDetector::WatershedDuneDetector(BasedEdgeImageProcessor* imgproc, const WatershedDuneDetectorParameters &params)
{
	ImageProcess = imgproc;
	Parameters = params;
}

WatershedDuneDetector::~WatershedDuneDetector()
{

}

std::vector<DuneSegment> WatershedDuneDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	//cv::imshow("Processed", processedImage);
	//cv::waitKey(0);

	std::vector<DuneSegment> duneSegs;
	std::vector<std::vector<cv::Point>> contours = GetContours(processedImage);

	//cv::Mat colorImg;
	//cv::cvtColor(img, colorImg, CV_GRAY2BGR);
	//cv::drawContours(colorImg, contours, -1, cv::Scalar(0, 0, 255), 2);


	for (size_t i = 0; i < contours.size(); ++i)
	{
		DuneSegment s;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			s.Data.push_back(DuneSegmentData(contours[i][j], 0));
			//cv::circle(colorImg, contours[i][j], 1, cv::Scalar(255, 0, 0), 2);
		}
		duneSegs.push_back(s);
	}

	//cv::imshow("Contours", colorImg);
	//cv::waitKey(0);

	return duneSegs;
}

std::vector<std::vector<cv::Point>> WatershedDuneDetector::GetContours(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int i = 0;
	std::vector<std::vector<cv::Point>>::iterator ctrIt = contours.begin();
	while (i < (int)contours.size())
	{
		if ((int)contours[i].size() < Parameters.MinSegmentLength)
		{
			//I for some reason don't remember how to use iterators...
			contours.erase(contours.begin() + i);
		}
		else
		{
			i++;
		}
	}

	std::vector<std::vector<cv::Point>> outputSegments = FilterSegmentsByGradients(contours);

	return outputSegments;
}

std::vector<std::vector<cv::Point>> WatershedDuneDetector::FilterSegmentsByGradients(std::vector<std::vector<cv::Point>> &contours)
{
	WatershedImageProcessor* wsImgProc = dynamic_cast<WatershedImageProcessor*>(ImageProcess);

	double domOrientation = wsImgProc->FindDominantOrientation(DominantOrientationMethod::HOG, Parameters.HistogramBins);

	std::vector<std::vector<cv::Point>> output;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		std::vector<cv::Point> segment;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			double dir = wsImgProc->BaseData.Gradient.at<cv::Vec4d>(contours[i][j])[GRADIENT_MAT_DIRECTION_INDEX];

			double diff = std::fabs(dir - domOrientation);
			if (diff < Parameters.AngleTolerance)
			{
				segment.push_back(contours[i][j]);
			}
		}

		if (segment.size() > 0)
		{
			output.push_back(segment);
		}
	}

	return output;
}

}