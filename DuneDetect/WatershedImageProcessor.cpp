#include "WatershedImageProcessor.h"

namespace dune
{

WatershedImageProcessor::WatershedImageProcessor()
{

}

WatershedImageProcessor::~WatershedImageProcessor()
{

}

WatershedImageProcessor::WatershedImageProcessor(const WatershedImageProcessor &cpy)
{

}

WatershedImageProcessor::WatershedImageProcessor(const WatershedProcessorParameters &params)
{

}

void WatershedImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	double k = 5;
	cv::Mat filtered;
	cv::medianBlur(inputImg, filtered, k);
	cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 4.7, (double)k / 4.7);

	
}

void WatershedImageProcessor::WatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	//First find the mean value
	cv::Scalar mean, stdev;
	cv::meanStdDev(inputImg, mean, stdev);


}

}