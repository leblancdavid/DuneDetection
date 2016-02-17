#include "GenericImageProcessing.h"
#include "GaussianScalePyramid.h"

namespace dune
{
	namespace imgproc
	{
		void GetSobelImage(const cv::Mat &input, cv::Mat &output, int k)
		{
			cv::Mat dx, dy, mag(input.rows, input.cols, CV_64F);
			cv::Sobel(input, dx, CV_64F, 1, 0, k);
			cv::Sobel(input, dy, CV_64F, 0, 1, k);
			for (int x = 0; x < dx.cols; ++x)
			{
				for (int y = 0; y < dx.rows; ++y)
				{
					mag.at<double>(y, x) = std::sqrt(dx.at<double>(y, x)*dx.at<double>(y, x) + dy.at<double>(y, x)*dy.at<double>(y, x));
				}
			}

			cv::normalize(mag, mag, 0, 255.0, cv::NORM_MINMAX);
			mag.convertTo(output, CV_8U);
		}

		void LocalScaleIntegralIlluminationNormalization(const cv::Mat &inputImg, cv::Mat &outputImg)
		{
			GaussianScalePyramid gsp;
			gsp.Process(inputImg);
			cv::Mat scaleMap = gsp.GetScaleMap();
			cv::Mat scaleImage = gsp.GetScaleImage();
			//cv::imshow("scaleImage", scaleImage);
			//cv::waitKey(0);

			cv::Mat integralImg;
			cv::integral(scaleImage, integralImg, CV_64F);

			cv::Scalar meanIntensity, stdDevIntensity;
			cv::meanStdDev(scaleImage, meanIntensity, stdDevIntensity);

			cv::Point A, B, C, D;
			outputImg = cv::Mat(scaleImage.rows, scaleImage.cols, CV_8UC1);

			cv::Mat diff(scaleImage.rows, scaleImage.cols, CV_64F);

			for (int x = 0; x < inputImg.cols; ++x)
			{
				for (int y = 0; y < inputImg.rows; ++y)
				{
					int radius = scaleMap.at<double>(y, x) * 6.0;
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

					//outputImg.at<uchar>(y, x) = (uchar)outputVal;

					diff.at<double>(y, x) = (sum / area) - meanIntensity[0];
				}
			}
			cv::normalize(diff, diff, 0, 255.0, cv::NORM_MINMAX);
			diff.convertTo(outputImg, CV_8U);
		}

		void IntegralIlluminationNormalization(const cv::Mat &inputImg, cv::Mat &outputImg, int radius)
		{
			cv::Mat integralImg;
			cv::integral(inputImg, integralImg, CV_64F);

			cv::Scalar meanIntensity, stdDevIntensity;
			cv::meanStdDev(inputImg, meanIntensity, stdDevIntensity);

			cv::Point A, B, C, D;
			outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC1);

			cv::Mat diff(inputImg.rows, inputImg.cols, CV_64F);

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

					//outputImg.at<uchar>(y, x) = (uchar)outputVal;

					diff.at<double>(y, x) = (sum / area) - meanIntensity[0];
				}
			}

			cv::normalize(diff, diff, 0, 255.0, cv::NORM_MINMAX);
			//cv::Mat outputVal;
			diff.convertTo(outputImg, CV_8U);
			//cv::imshow("diffImg", outputImg);
			//cv::waitKey(0);

		}

		void IntegralEdgeThreshold(const cv::Mat &inputImg, cv::Mat &outputImg, int radius, int k)
		{
			cv::Mat dx, dy, dMag(inputImg.rows, inputImg.cols, CV_64F);
			cv::Sobel(inputImg, dx, CV_64F, 1, 0, k);
			cv::Sobel(inputImg, dy, CV_64F, 0, 1, k);
			for (int x = 0; x < dx.cols; ++x)
			{
				for (int y = 0; y < dx.rows; ++y)
				{
					dMag.at<double>(y, x) = std::sqrt(dx.at<double>(y, x)*dx.at<double>(y, x) + dy.at<double>(y, x)*dy.at<double>(y, x)) / 1000.0;
				}
			}

			cv::Scalar mean, stddev;
			cv::meanStdDev(dMag, mean, stddev);

			cv::Mat smooth;
			double g = (radius * 2 + 1);
			cv::GaussianBlur(dMag, smooth, cv::Size((int)g, (int)g), g / 5.0, g / 5.0);
			cv::normalize(smooth, smooth, 0, 255, cv::NORM_MINMAX);
			smooth.convertTo(outputImg, CV_8U);

			//double scalingFactor = 255.0 / (mean[0] * 2.0);
			//cv::Mat integralImg;
			//cv::integral(dMag, integralImg, CV_64F);
			//cv::Point A, B, C, D;
			//outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC1);

			//for (int x = 0; x < inputImg.cols; ++x)
			//{
			//	for (int y = 0; y < inputImg.rows; ++y)
			//	{
			//		int left = x - radius;
			//		int right = x + radius;
			//		int top = y - radius;
			//		int bottom = y + radius;

			//		if (left < 0)
			//			left = 0;
			//		if (right >= integralImg.cols)
			//			right = integralImg.cols - 1;
			//		if (top < 0)
			//			top = 0;
			//		if (bottom >= integralImg.rows)
			//			bottom = integralImg.rows - 1;

			//		int width = right - left;
			//		int height = bottom - top;
			//		double area = width * height;

			//		A.x = left, A.y = top;
			//		B.x = right, B.y = top;
			//		C.x = left, C.y = bottom;
			//		D.x = right, D.y = bottom;

			//		double sum = integralImg.at<double>(D) -integralImg.at<double>(C) -integralImg.at<double>(B) +integralImg.at<double>(A);
			//		sum /= area;
			//		/*double outputVal;
			//		if (sum > mean[0])
			//			outputVal = 255;
			//		else
			//			outputVal = 0;*/
			//		double outputVal = sum*scalingFactor;
			//		if (outputVal > 255.0)
			//			outputVal = 255;
			//		outputImg.at<uchar>(y, x) = (uchar)outputVal;

			//		
			//		//diff.at<double>(y, x) = (sum / area) - meanIntensity[0];
			//	}
			//}

			cv::adaptiveThreshold(outputImg, outputImg, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 101, -10);

		}

		void ThinCannyEdges(const cv::Mat &cannyImg, cv::Mat &outputImg)
		{
			outputImg = cannyImg.clone();

			for (int x = 0; x < outputImg.cols; ++x)
			{
				for (int y = 0; y < outputImg.rows; ++y)
				{
					if (outputImg.at<uchar>(y, x))
					{
						int left = x - 1;
						int top = y - 1;
						int right = x + 1;
						int bottom = y + 1;

						if (left > 0)
						{
							if (top > 0 && outputImg.at<uchar>(top, left))
							{
								outputImg.at<uchar>(y, left) = 0;
								outputImg.at<uchar>(top, x) = 0;
							}
							if (bottom < outputImg.rows && outputImg.at<uchar>(bottom, left))
							{
								outputImg.at<uchar>(y, left) = 0;
								outputImg.at<uchar>(bottom, x) = 0;

							}
						}
						if (right < outputImg.cols)
						{
							if (top > 0 && outputImg.at<uchar>(top, right))
							{
								outputImg.at<uchar>(y, right) = 0;
								outputImg.at<uchar>(top, x) = 0;
							}
							if (bottom < outputImg.rows && outputImg.at<uchar>(bottom, right))
							{
								outputImg.at<uchar>(y, right) = 0;
								outputImg.at<uchar>(bottom, x) = 0;

							}
						}
					}
				}
			}

			/*cv::imwrite("Canny.png", cannyImg);
			cv::imwrite("ThinnedCanny.png", outputImg);
			cv::imshow("Canny", cannyImg);
			cv::imshow("Thinned Canny", outputImg);
			cv::waitKey(0);*/
		}

		std::vector<double> ExtractHoG(const cv::Mat &img, int bins, int K)
		{
			cv::Mat sobel_x, sobel_y;

			cv::Sobel(img, sobel_x, CV_64F, 1, 0, K);
			cv::Sobel(img, sobel_y, CV_64F, 0, 1, K);
			cv::Scalar u_x, u_y, sig_x, sig_y;
			cv::meanStdDev(sobel_x, u_x, sig_x);
			cv::meanStdDev(sobel_y, u_y, sig_y);

			double avgOrientation = std::atan2(u_y[0], u_x[0]);
			double increments = 2.0*3.1416 / (double)bins;
			std::vector<double> HoG(bins);
			std::vector<cv::Point2d> HoG2D(bins);

			//Compute the normalized orientation.
			for (int x = 0; x < sobel_x.cols; ++x)
			{
				for (int y = 0; y < sobel_x.rows; ++y)
				{
					double dx = sobel_x.at<double>(y, x);
					double dy = sobel_y.at<double>(y, x);

					double orientation = std::atan2(dy, dx) - avgOrientation;
					while (orientation < 0)
						orientation += 2.0*3.1416;

					int bin = std::ceil((orientation / increments) - 0.5);
					if (bin >= bins)
						bin = 0;

					HoG2D[bin].x += dx / 1000.0;
					HoG2D[bin].y += dy / 1000.0;
				}
			}

			double max = DBL_MIN;
			for (size_t i = 0; i < HoG.size(); ++i)
			{
				double dx = HoG2D[i].x / 1000.0;
				double dy = HoG2D[i].y / 1000.0;
				HoG[i] = std::sqrt(dx*dx + dy*dy);
				if (HoG[i] > max)
				{
					max = HoG[i];
				}
			}
			for (size_t i = 0; i < HoG.size(); ++i)
			{
				HoG[i] /= max;
			}

			return HoG;
		}


	}

}

