#include "DuneSegmentDetector.h"

namespace dune
{

DuneSegmentDetector::DuneSegmentDetector() 
{
	ImageProcess = new AdaptiveImageProcessor();
}

DuneSegmentDetector::DuneSegmentDetector(BaseImageProcessor* imgproc, const DuneSegmentDetectorParameters &params) 
{
	ImageProcess = imgproc;
	Parameters = params;
}

DuneSegmentDetector::~DuneSegmentDetector() 
{
	delete ImageProcess;
}

std::vector<DuneSegment> DuneSegmentDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	std::vector<DuneSegment> duneSegs;
	std::vector<std::vector<cv::Point>> contours = GetContours(processedImage);

	cv::Mat colorImg;
	cv::cvtColor(img, colorImg, CV_GRAY2BGR);
	cv::drawContours(colorImg, contours, -1, cv::Scalar(0,0,255),2);
	cv::imshow("Contours", colorImg);
	cv::waitKey(0);

	return duneSegs;
}

std::vector<std::vector<cv::Point>> DuneSegmentDetector::GetContours(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	int i = 0;
	std::vector<std::vector<cv::Point>>::iterator ctrIt = contours.begin();
	while(i < (int)contours.size())
	{
		if((int)contours[i].size() < Parameters.MinContourLength)
		{
			//I for some reason don't remember how to use iterators...
			contours.erase(contours.begin()+i);
		}
		else
		{
			i++;
		}
	}

	return contours;
}

}