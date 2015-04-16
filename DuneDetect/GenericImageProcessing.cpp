#include "GenericImageProcessing.h"

namespace dune
{
	namespace imgproc
	{

		void IntegralIlluminationNormalization(const cv::Mat &inputImg, cv::Mat &outputImg, int radius)
		{
			cv::Mat integralImg;
			cv::integral(inputImg, integralImg, CV_64F);

			cv::Scalar meanIntensity, stdDevIntensity;
			cv::meanStdDev(inputImg, meanIntensity, stdDevIntensity);

			cv::Point A, B, C, D;
			outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC1);

			for (int x = 0; x < inputImg.cols; ++x)
			{
				for (int y = 0; y < inputImg.rows; ++y)
				{
					int left = x - radius;
					int right = x + radius;
					int top = y - radius;
					int bottom = y + radius;

					if (left < 0)
						left = 0;
					if (right >= integralImg.cols)
						right = integralImg.cols - 1;
					if (top < 0)
						top = 0;
					if (bottom >= integralImg.rows)
						bottom = integralImg.rows - 1;

					int width = right - left;
					int height = bottom - top;
					double area = width * height;

					A.x = left, A.y = top;
					B.x = right, B.y = top;
					C.x = left, C.y = bottom;
					D.x = right, D.y = bottom;

					double sum = integralImg.at<double>(D) -integralImg.at<double>(C) -integralImg.at<double>(B) +integralImg.at<double>(A);

					double r = meanIntensity[0] / (sum / area);
					double outputVal = std::ceil((double)inputImg.at<uchar>(y, x) * r - 0.5);
					if (outputVal > 255.0)
						outputVal = 255.0;
					else if (outputVal < 0)
						outputVal = 0;

					outputImg.at<uchar>(y, x) = (uchar)outputVal;
				}
			}

		}


	}

}

