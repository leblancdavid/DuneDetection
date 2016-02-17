#ifndef _DUNE_SIFT_FEATURE_EXTRACTOR_H_
#define _DUNE_SIFT_FEATURE_EXTRACTOR_H_

#include "BaseFeatureExtractor.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <nonfree\nonfree.hpp>

namespace duneML
{
	class SIFTParameters
	{
	public:

		SIFTParameters()
		{
			NumFeatures = 0;
			NumOctaves = 3;
			ContrastThreshold = 0.04;
			K = 5;
			EdgeThreshold = 10.0;
			Sigma = 1.6;
		}
		~SIFTParameters() {}
		SIFTParameters(const SIFTParameters &cpy)
		{
			NumFeatures = cpy.NumFeatures;
			NumOctaves = cpy.NumOctaves;
			ContrastThreshold = cpy.ContrastThreshold;
			K = cpy.K;
			EdgeThreshold = cpy.EdgeThreshold;
			Sigma = cpy.Sigma;
		}

		int NumFeatures;
		int NumOctaves;
		int K;
		double ContrastThreshold;
		double EdgeThreshold;
		double Sigma;
	};



	class DuneSIFTFeatureExtractor : public BaseFeatureExtractor
	{
	public:
		DuneSIFTFeatureExtractor()
		{
			siftDetector = cv::SIFT(parameters.NumFeatures,
				parameters.NumOctaves,
				parameters.ContrastThreshold,
				parameters.EdgeThreshold,
				parameters.Sigma);
		}

		DuneSIFTFeatureExtractor(const SIFTParameters &params)
		{
			parameters = params;
			siftDetector = cv::SIFT(parameters.NumFeatures,
				parameters.NumOctaves,
				parameters.ContrastThreshold,
				parameters.EdgeThreshold,
				parameters.Sigma);
		}

		DuneSIFTFeatureExtractor(const DuneSIFTFeatureExtractor &cpy)
		{
			parameters = cpy.parameters;
			siftDetector = cv::SIFT(parameters.NumFeatures,
				parameters.NumOctaves,
				parameters.ContrastThreshold,
				parameters.EdgeThreshold,
				parameters.Sigma);
		}
		~DuneSIFTFeatureExtractor() {}

		void Process(const cv::Mat &img, std::vector<cv::KeyPoint> &points, cv::Mat &descriptor, const cv::Mat &scaleMat = cv::Mat()) const
		{
			cv::Mat dx, dy;
			cv::Sobel(img, dx, CV_64F, 1, 0, parameters.K);
			cv::Sobel(img, dy, CV_64F, 0, 1, parameters.K);

			cv::Mat scaleVals = scaleMat;
			if (scaleVals.rows == 0 || scaleVals.cols == 0)
			{
				scaleVals = cv::Mat(img.rows, img.cols, CV_64F);
				scaleVals = cv::Scalar(parameters.Sigma);
			}

			for (size_t i = 0; i < points.size(); ++i)
			{
				double patchSigma = scaleVals.at<double>(points[i].pt)*3.0;
				double patchHalf = patchSigma / 2.0;
				cv::Rect region = cv::Rect(points[i].pt.x - patchHalf,
					points[i].pt.y - patchHalf,
					patchSigma, patchSigma);
				double angle = getDominantOrientation(dx, dy, region);
				points[i].angle = angle;
				points[i].size = patchSigma;
			}

			siftDetector(img, cv::Mat(), points, descriptor, true);
			points = points;
		}

	private:
		SIFTParameters parameters;
		cv::SIFT siftDetector;
	};
}

#endif