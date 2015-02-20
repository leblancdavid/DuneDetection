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
	cv::Mat smoothed, equalized;
	cv::medianBlur(inputImg, smoothed, 21);
	cv::equalizeHist(smoothed, equalized);
	cv::adaptiveThreshold(equalized, outputImg, 255, parameters.Method, parameters.ThresholdType, parameters.BlockSize, parameters.C);
	cv::erode(outputImg, outputImg, cv::Mat(), cv::Point(-1,-1), 1);
	cv::dilate(outputImg, outputImg, cv::Mat(), cv::Point(-1,-1), 1);

	//cv::imshow("AdaptiveTheshold Result", outputImg);
	//cv::waitKey(0);

}

}