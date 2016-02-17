#pragma once

#include "BaseFeatureExtractor.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <nonfree\nonfree.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace duneML
{
	class DuneORBFeatureExtractor : public BaseFeatureExtractor
	{
	public:
		DuneORBFeatureExtractor()
		{

		}

		~DuneORBFeatureExtractor() {}

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
				double patchSigma = (scaleVals.at<double>(points[i].pt)+5.0)*6;
				double patchHalf = patchSigma / 2.0;
				cv::Rect region = cv::Rect(points[i].pt.x - patchHalf,
					points[i].pt.y - patchHalf,
					patchSigma, patchSigma);
				double angle = getDominantOrientation(dx, dy, region);
				points[i].angle = angle;
				points[i].size = patchSigma;
			}

			cv::Mat orbDescriptors;
			orbDetector(img, cv::Mat(), points, orbDescriptors, true);
			descriptor = convertOrbDescriptors(orbDescriptors);
			points = points;
		}

	private:
		cv::OrbFeatureDetector orbDetector;

		cv::Mat convertOrbDescriptors(const cv::Mat &orbDescriptors) const
		{
			cv::Mat descriptors(orbDescriptors.rows, orbDescriptors.cols * 8, CV_64F);

			for (int i = 0; i < orbDescriptors.rows; ++i)
			{
				for (int j = 0; j < orbDescriptors.cols; ++j)
				{
					char orbVal = orbDescriptors.at<uchar>(i, j);
					for (int k = 0; k < 8; ++k)
					{
						descriptors.at<double>(i, j * 8 + k) = (orbVal >> k) & 1;
					}
				}
			}

			return descriptors;
		}
	};
}
