#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_

#include "OpenCVHeaders.h"

namespace dune
{
	class ImageProcessor
	{
	public:

		ImageProcessor() {};
		ImageProcessor(const ImageProcessor &cpy) {};
		~ImageProcessor() {};

		virtual void Process(const cv::Mat &inputImg, const cv::Mat &outputImg)=0;
		
	};

}

#endif