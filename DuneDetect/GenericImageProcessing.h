#ifndef _GENERIC_IMAGE_PROCESSING_H_
#define _GENERIC_IMAGE_PROCESSING_H_

#include "OpenCVHeaders.h"

namespace dune
{
	namespace imgproc
	{
		void IntegralIlluminationNormalization(const cv::Mat &inputImg, cv::Mat &outputImg, int radius);
		void IntegralEdgeThreshold(const cv::Mat &inputImg, cv::Mat &outputImg, int radius, int k);

		///This is a hacky fix to the canny edges, this function ensures that an edge always has 1 or 2 neighboring edges
		///So that the endpoint of an edge will only have 1 neighboring edge.
		void ThinCannyEdges(const cv::Mat &cannyImg, cv::Mat &outputImg);

		std::vector<double> ExtractHoG(const cv::Mat &img, int bins, int K);

	}

}


#endif