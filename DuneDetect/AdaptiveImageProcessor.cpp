#include "AdaptiveImageProcessor.h"

namespace dune
{
AdaptiveImageProcessor::AdaptiveImageProcessor()
{

}

AdaptiveImageProcessor::~AdaptiveImageProcessor()
{

}

AdaptiveImageProcessor::AdaptiveImageProcessor(const AdaptiveImageProcessor &cpy)
{
	parameters = cpy.parameters;
}

AdaptiveImageProcessor::AdaptiveImageProcessor(const AdaptiveImageProcParams &params)
{
	parameters = params;
}

void AdaptiveImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	cv::Mat smoothed;
	cv::Mat equalized(inputImg.rows, inputImg.cols, CV_8UC1);
	cv::medianBlur(inputImg, smoothed, 5);
	//cv::equalizeHist(smoothed, equalized);

	double avgIntensity = 0;
	double stdDevIntensity = 0;
	for(int x = 0; x < smoothed.cols; ++x)
	{
		for(int y = 0; y < smoothed.rows; ++y)
		{
			avgIntensity += (double)smoothed.at<uchar>(y,x)/255.0;
		}
	}

	avgIntensity /= (double)(smoothed.cols*smoothed.rows)/255.0;

	for(int x = 0; x < smoothed.cols; ++x)
	{
		for(int y = 0; y < smoothed.rows; ++y)
		{
			stdDevIntensity += fabs(((double)smoothed.at<uchar>(y,x) - avgIntensity))/255.0;
		}
	}

	stdDevIntensity /= (double)(smoothed.cols*smoothed.rows)/255.0;

	double scaler = 30.0/stdDevIntensity;
	double q = -1.0;
	for(int x = 0; x < smoothed.cols; ++x)
	{
		for(int y = 0; y < smoothed.rows; ++y)
		{
			double val = (double)(smoothed.at<uchar>(y,x) - avgIntensity - (q*stdDevIntensity))*scaler;
			if(val < 0)
				equalized.at<uchar>(y,x) = 0;
			else
				equalized.at<uchar>(y,x) = (uchar)val;
		}
	}

	//cv::imshow("Equalized", equalized);
	//cv::waitKey(0);
	//double q = 0.0;
	//cv::Mat thresholdImg;
	
	//cv::threshold(inputImg, thresholdImg, avgIntensity + (q*stdDevIntensity), 255, CV_THRESH_BINARY);

	cv::adaptiveThreshold(equalized, outputImg, 255, parameters.Method, parameters.ThresholdType, parameters.BlockSize, parameters.C);
	cv::erode(outputImg, outputImg, cv::Mat(), cv::Point(-1,-1), 1);
	cv::dilate(outputImg, outputImg, cv::Mat(), cv::Point(-1,-1), 1);

	/*cv::Mat smoothed, equalized;
	cv::medianBlur(inputImg, smoothed, 21);
	cv::equalizeHist(smoothed, equalized);
	cv::adaptiveThreshold(equalized, outputImg, 255, parameters.Method, parameters.ThresholdType, parameters.BlockSize, parameters.C);
	cv::erode(outputImg, outputImg, cv::Mat(), cv::Point(-1,-1), 1);
	cv::dilate(outputImg, outputImg, cv::Mat(), cv::Point(-1,-1), 1);*/
}

}