#ifndef _ADAPTIVE_IMAGE_PROCESSOR_H_
#define _ADAPTIVE_IMAGE_PROCESSOR_H_

#include "BaseImageProcessor.h"

namespace dune 
{
class AdaptiveImageProcParams 
{
public:
	AdaptiveImageProcParams()
	{
		BlockSize = 31;
		C = -5.0;
		Method = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
		ThresholdType = cv::THRESH_BINARY;
	}
	~AdaptiveImageProcParams() {}
	AdaptiveImageProcParams(const AdaptiveImageProcParams &cpy)
	{
		BlockSize = cpy.BlockSize;
		C = cpy.C;
		Method = cpy.Method;
		ThresholdType = cpy.ThresholdType;
	}
	AdaptiveImageProcParams(int pMethod, int pThreshType, double pC, int pBlockSize)
	{
		Method = pMethod;
		ThresholdType = pThreshType;
		C = pC;
		BlockSize = pBlockSize;
	}

	int Method;
	int ThresholdType;
	double C;
	int BlockSize;
};

class AdaptiveImageProcessor : public BaseImageProcessor
{
public:
	AdaptiveImageProcessor();
	~AdaptiveImageProcessor();
	AdaptiveImageProcessor(const AdaptiveImageProcessor &cpy);
	AdaptiveImageProcessor(const AdaptiveImageProcParams &params);

	void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

private:
	AdaptiveImageProcParams parameters;

};



}

#endif