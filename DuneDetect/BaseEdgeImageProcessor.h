#ifndef _BASE_EDGE_IMAGE_PROCESSOR_H_
#define _BASE_EDGE_IMAGE_PROCESSOR_H_

#include "BaseImageProcessor.h"

namespace dune
{
	struct ImageGradientData
	{
		cv::Mat Gradient;
		cv::Scalar Mean;
		cv::Scalar StdDev;
	};

	class BasedEdgeImageProcessor : public BaseImageProcessor
	{
	public:
		BasedEdgeImageProcessor() {};
		~BasedEdgeImageProcessor() {};
		BasedEdgeImageProcessor(const BasedEdgeImageProcessor &cpy) {};

		virtual void Process(const cv::Mat &inputImg, cv::Mat &outputImg) = 0;

		void ComputeGradient(const cv::Mat &inputImg, int K)
		{
			BaseData.Gradient = cv::Mat(inputImg.rows, inputImg.cols, CV_64FC4);
			cv::Mat dx, dy;
			cv::Sobel(inputImg, dx, CV_64F, 1, 0, K);
			cv::Sobel(inputImg, dy, CV_64F, 0, 1, K);

			for (int x = 0; x < inputImg.cols; ++x)
			{
				for (int y = 0; y < inputImg.rows; ++y)
				{
					cv::Vec4d g;
					g[GRADIENT_MAT_DX_INDEX] = dx.at<double>(y, x);
					g[GRADIENT_MAT_DY_INDEX] = dy.at<double>(y, x);
					g[GRADIENT_MAT_MAGNITUDE_INDEX] = std::sqrt(g[GRADIENT_MAT_DX_INDEX] * g[GRADIENT_MAT_DX_INDEX] +
																g[GRADIENT_MAT_DY_INDEX] * g[GRADIENT_MAT_DY_INDEX]);
					g[GRADIENT_MAT_DIRECTION_INDEX] = std::atan2(g[GRADIENT_MAT_DY_INDEX], g[GRADIENT_MAT_DX_INDEX]);
					BaseData.Gradient.at<cv::Vec4d>(y, x) = g;
				}
			}

			cv::meanStdDev(BaseData.Gradient, BaseData.Mean, BaseData.StdDev);
		}

	protected:
		const int GRADIENT_MAT_DX_INDEX = 0;
		const int GRADIENT_MAT_DY_INDEX = 1;
		const int GRADIENT_MAT_MAGNITUDE_INDEX = 2;
		const int GRADIENT_MAT_DIRECTION_INDEX = 3;

		//The gradient matrix is a 4-channel matrix where 
		//C1: dx, C2: dy, C3: magnitude, C4: direction 
		ImageGradientData BaseData;
		
	private:

	};



}

#endif