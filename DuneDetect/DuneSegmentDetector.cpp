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
	/*if(ImageProcess != NULL)
	{
		delete ImageProcess;
	}*/
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

std::vector<std::vector<cv::Point>> DuneSegmentDetector::SplitContourSegments(const std::vector<cv::Point> &contour)
{
	std::vector<std::vector<cv::Point>> segments;
	int k = 9;
	std::vector<double> kernal(k);
	kernal[0] = -1.0;
	kernal[1] = -2.0;
	kernal[2] = -3.0;
	kernal[3] = 1.0;
	kernal[4] = 6.0;;
	kernal[5] = 1.0;
	kernal[6] = -3.0;
	kernal[7] = -2.0;
	kernal[8] = -1.0;


	return segments;


}

}