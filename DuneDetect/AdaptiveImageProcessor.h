#ifndef _ADAPTIVE_IMAGE_PROCESSOR_H_
#define _ADAPTIVE_IMAGE_PROCESSOR_H_

#include "ImageProcessor.h"

namespace dune 
{
struct AdaptiveImageProcParams 
{

};

class AdaptiveImageProcessor : public ImageProcessor
{
public:
	AdaptiveImageProcessor();
	~AdaptiveImageProcessor();
	AdaptiveImageProcessor(const AdaptiveImageProcessor &cpy);
	AdaptiveImageProcessor(const AdaptiveImageProcParams &params);

	void Process(const cv::Mat &inputImg, const cv::Mat &outputImg);

private:
	AdaptiveImageProcParams parameters;

};



}

#endif