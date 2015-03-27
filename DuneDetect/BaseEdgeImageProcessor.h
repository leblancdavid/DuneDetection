#ifndef _BASE_EDGE_IMAGE_PROCESSOR_H_
#define _BASE_EDGE_IMAGE_PROCESSOR_H_

#include "BaseImageProcessor.h"

namespace dune
{

	class BasedEdgeImageProcessor : public BaseImageProcessor
	{
	public:
		BasedEdgeImageProcessor() {};
		~BasedEdgeImageProcessor() {};
		BasedEdgeImageProcessor(const BasedEdgeImageProcessor &cpy) {};

		virtual void Process(const cv::Mat &inputImg, cv::Mat &outputImg) = 0;

		double GetMagnitude(cv::Point &at)
		{
			double x = xDeriv.at<double>(at);
			double y = yDeriv.at<double>(at);

			return std::sqrt(x*x + y*y);
		}

		double GetDirection(cv::Point &at)
		{
			double x = xDeriv.at<double>(at);
			double y = yDeriv.at<double>(at);

			return std::atan2(y,x);
		}

	protected:
		cv::Mat xDeriv;
		cv::Mat yDeriv;
	private:

	};



}

#endif