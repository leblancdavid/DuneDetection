#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_

#include "OpenCVHeaders.h"

namespace dune
{
	class BaseImageProcessor
	{
	public:

		BaseImageProcessor() {};
		BaseImageProcessor(const BaseImageProcessor &cpy) {};
		~BaseImageProcessor() {};

		virtual void Process(const cv::Mat &inputImg, cv::Mat &outputImg)=0;
		
	};

}

#endif