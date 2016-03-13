#ifndef _BASE_FEATURE_CLASSIFER_H_
#define _BASE_FEATURE_CLASSIFER_H_

#include "OpenCVHeaders.h"

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

		virtual float Predict(const cv::Mat& example, bool normalize = true) { return 0; }

		enum NormalizationType {MINMAX, MEANSCALE};

	private:

	protected:
		double meanNormalizer, scaleNormalizer;
		NormalizationType normType;

		virtual cv::Mat NormalizeMat(const cv::Mat &example)
		{
			cv::Mat normalized(example.rows, example.cols, CV_32F);
			for (int i = 0; i < example.rows; ++i)
			{
				if (normType == MEANSCALE)
				{
					for (int j = 0; j < example.cols; ++j)
					{
						float normVal = (example.at<float>(i, j) - meanNormalizer) / scaleNormalizer;
						normalized.at<float>(i, j) = normVal;
					}
				}
				else if (normType == MINMAX)
				{
					float min = FLT_MAX, max = -1.0 * FLT_MAX;
					for (int j = 0; j < example.cols; ++j)
					{
						float val = example.at<float>(i, j);
						if (val > max)
							max = val;
						if (val < min)
							min = val;
					}
					
					for (int j = 0; j < example.cols; ++j)
					{
						normalized.at<float>(i, j) = (example.at<float>(i, j) - min)/(max - min);
						//std::cout << normalized.at<float>(i, j) << "\t" << example.at<float>(i, j) << std::endl;
					}
					//std::cout << std::endl;
				}
				
			}

			return normalized;
		}
	};
}

#endif