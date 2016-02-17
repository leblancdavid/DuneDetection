#ifndef _CLASSIFIER_OUTPUT_FILTERING_H_
#define _CLASSIFIER_OUTPUT_FILTERING_H_

#include "OpenCVHeaders.h"

namespace duneML
{
	//Expects a CV_32F Mat
	cv::Mat Thin(const cv::Mat &input, float lowerThreshold, float upperThreshold)
	{
		std::vector<cv::Point> anchors;
		for (int x = 1; x < input.cols-1; ++x)
		{
			for (int y = 1; y < input.rows-1; ++y)
			{


			}
		}
	}

	
}

#endif