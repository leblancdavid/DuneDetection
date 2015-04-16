#ifndef _GENERIC_IMAGE_PROCESSING_H_
#define _GENERIC_IMAGE_PROCESSING_H_

#include "OpenCVHeaders.h"

namespace dune
{
	namespace imgproc
	{
		void IntegralIlluminationNormalization(const cv::Mat &inputImg, cv::Mat &outputImg, int radius);

	}

}


#endif