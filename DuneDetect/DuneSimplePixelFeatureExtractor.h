#ifndef _DUNE_SIMPLE_PIXEL_FEATURE_EXTRACTOR_H_
#define _DUNE_SIMPLE_PIXEL_FEATURE_EXTRACTOR_H_

#include "BaseFeatureExtractor.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <nonfree\nonfree.hpp>

namespace duneML
{
	class SimplePixelParameters
	{
	public:

		SimplePixelParameters()
		{
			WindowSize = 11;
			K = 5;
		}
		~SimplePixelParameters() {}
		SimplePixelParameters(const SimplePixelParameters &cpy)
		{
			WindowSize = cpy.WindowSize;
			K = cpy.K;
		}

		double WindowSize;
		int K;
	};



	class DuneSimplePixelFeatureExtractor : public BaseFeatureExtractor
	{
	public:
		DuneSimplePixelFeatureExtractor()
		{

		}

		DuneSimplePixelFeatureExtractor(const SimplePixelParameters &params)
		{
			parameters = params;
		}

		DuneSimplePixelFeatureExtractor(const DuneSimplePixelFeatureExtractor &cpy)
		{
			parameters = cpy.parameters;
		}
		~DuneSimplePixelFeatureExtractor() {}

		void Process(const cv::Mat &img, std::vector<cv::KeyPoint> &points, cv::Mat &descriptor) const
		{
			cv::Mat dx, dy;
			cv::Sobel(img, dx, CV_64F, 1, 0, parameters.K);
			cv::Sobel(img, dy, CV_64F, 0, 1, parameters.K);

			std::vector<cv::KeyPoint> filteredPoints;
			double rotImgSize = parameters.WindowSize*1.5;
			double rotImgHalf = rotImgSize / 2.0;
			for (size_t i = 0; i < points.size(); ++i)
			{
				if (points[i].pt.x - rotImgHalf < 0 ||
					points[i].pt.x + rotImgHalf >= img.cols ||
					points[i].pt.y - rotImgHalf < 0 ||
					points[i].pt.y + rotImgHalf >= img.rows)
					continue;
				filteredPoints.push_back(points[i]);
			}

			descriptor = cv::Mat(filteredPoints.size(), parameters.WindowSize*parameters.WindowSize, CV_32F);

			for (size_t i = 0; i < filteredPoints.size(); ++i)
			{
				cv::KeyPoint keypoint;
				cv::Rect region = cv::Rect(filteredPoints[i].pt.x - rotImgHalf,
					filteredPoints[i].pt.y - rotImgHalf,
					rotImgSize, rotImgSize);
				double angle = getDominantOrientation(dx, dy, region);
				filteredPoints[i].angle = angle;
				
				double rotAngle = angle * 180.0 / 3.1416;
				if (rotAngle < 0.0)
					rotAngle += 360.0;

				//rotAngle *= -1.0;

				cv::Mat rotMat = cv::getRotationMatrix2D(cv::Point2f(rotImgHalf, rotImgHalf), rotAngle, 1.0f);
				
				
				cv::Mat roi = img(region);

				cv::Mat rotated, cropped;

				cv::warpAffine(roi, rotated, rotMat, roi.size());

				cv::getRectSubPix(rotated, cv::Size(parameters.WindowSize, parameters.WindowSize), cv::Point2f(rotImgHalf, rotImgHalf), cropped);

				cv::Scalar mean, stddev;
				cv::meanStdDev(cropped, mean, stddev);

				int j = 0;
				for (int x = 0; x < cropped.cols; ++x)
				{
					for (int y = 0; y < cropped.rows; ++y)
					{
						//float normVal = ((float)cropped.at<uchar>(y, x) - (float)mean[0]) / (float)stddev[0];
						float normVal = (float)cropped.at<uchar>(y, x);
						descriptor.at<float>(i, j) = normVal;

						++j;
					}
				}

				/*cv::Mat colorImg;
				cv::cvtColor(img, colorImg, CV_GRAY2BGR);
				cv::circle(colorImg, filteredPoints[i], 2, cv::Scalar(0, 0, 255));
				cv::imshow("Point", colorImg);
				cv::imshow("roi", roi);
				cv::imshow("cropped", cropped);
				cv::waitKey(0);*/

			}

			points = filteredPoints;
		}

	private:
		SimplePixelParameters parameters;

		

	};
}

#endif