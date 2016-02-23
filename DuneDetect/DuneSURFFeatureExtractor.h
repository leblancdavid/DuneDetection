#pragma once

#include "BaseFeatureExtractor.h"

#include "OpenCVHeaders.h"

namespace duneML
{
	class DuneSURFFeatureExtractor : public BaseFeatureExtractor
	{
	public:
		DuneSURFFeatureExtractor()
		{
	
		}

		DuneSURFFeatureExtractor(double hessian)
		{
			//surfDetector = cv::SurfFeatureDetector(hessian);
		}

		DuneSURFFeatureExtractor(const DuneSURFFeatureExtractor &cpy)
		{

		}
		~DuneSURFFeatureExtractor() {}

		void Process(const cv::Mat &img, std::vector<cv::KeyPoint> &points, cv::Mat &descriptor, const cv::Mat &scaleMat = cv::Mat()) const
		{
			cv::Mat dx, dy;
			int k = 5;
			double sigma = 1.6;
			cv::Sobel(img, dx, CV_64F, 1, 0, k);
			cv::Sobel(img, dy, CV_64F, 0, 1, k);

			cv::Mat scaleVals = scaleMat;
			if (scaleVals.rows == 0 || scaleVals.cols == 0)
			{
				scaleVals = cv::Mat(img.rows, img.cols, CV_64F);
				scaleVals = cv::Scalar(sigma);
			}

			for (size_t i = 0; i < points.size(); ++i)
			{
				double patchSigma = scaleVals.at<double>(points[i].pt)*6.0;
				double patchHalf = patchSigma / 2.0;
				cv::Rect region = cv::Rect(points[i].pt.x - patchHalf,
					points[i].pt.y - patchHalf,
					patchSigma, patchSigma);
				double angle = getDominantOrientation(dx, dy, region);
				points[i].angle = angle;
				points[i].size = patchSigma;
			}

			//surfDetector(img, cv::Mat(), points, descriptor, true);
			points = points;
		}

	private:
		//cv::SurfFeatureDetector surfDetector;
	};
}
