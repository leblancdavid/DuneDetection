#ifndef _RANDOM_TREE_FEATURE_CLASSIFER_H_
#define _RANDOM_TREE_FEATURE_CLASSIFER_H_

#include "BaseFeatureClassifier.h"

namespace duneML
{
	class RandomTreeFeatureClassifier : public BaseFeatureClassifier
	{
	public:
		RandomTreeFeatureClassifier()
		{
			meanNormalizer = 0.0;
			scaleNormalizer = 1.0;
		}
		~RandomTreeFeatureClassifier() {}

		TrainingResults Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples);

		float Predict(const cv::Mat& example, bool normalize=true);

	private:
		//CvRTrees rt;
		cv::Ptr<cv::ml::RTrees> rt;
	};
}

#endif