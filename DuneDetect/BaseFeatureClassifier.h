#ifndef _BASE_FEATURE_CLASSIFER_H_
#define _BASE_FEATURE_CLASSIFER_H_

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

namespace duneML
{
	class TrainingResults
	{
	public:
		TrainingResults()
		{
			TrueNegativeRate = 0.0;
			TruePositiveRate = 0.0;
			FalseNegativeRate = 0.0;
			FalsePositiveRate = 0.0;
		}
		~TrainingResults() {}
		TrainingResults(const TrainingResults &cpy)
		{
			TrueNegativeRate = cpy.TrueNegativeRate;
			TruePositiveRate = cpy.TruePositiveRate;
			FalseNegativeRate = cpy.FalseNegativeRate;
			FalsePositiveRate = cpy.FalsePositiveRate;
		}

		double TruePositiveRate;
		double FalsePositiveRate;
		double TrueNegativeRate;
		double FalseNegativeRate;
	};

	class BaseFeatureClassifier
	{
	public:
		BaseFeatureClassifier() {}
		virtual ~BaseFeatureClassifier() {}
		
		virtual TrainingResults Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples) { return TrainingResults(); }

		virtual float Predict(const cv::Mat& example) { return 0; }

	private:

	protected:
		double meanNormalizer, scaleNormalizer;

		virtual cv::Mat NormalizeMat(const cv::Mat &example)
		{
			cv::Mat normalized(example.rows, example.cols, CV_32F);
			for (int i = 0; i < example.rows; ++i)
			{
				for (int j = 0; j < example.cols; ++j)
				{
					float normVal = (example.at<float>(i, j) - meanNormalizer) / scaleNormalizer;
					normalized.at<float>(i, j) = normVal;
				}
			}

			return normalized;
		}
	};
}

#endif