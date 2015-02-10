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

void AdaptiveImageProcessor::Process(const cv::Mat &inputImg, const cv::Mat &outputImg)
{

}

}