#ifndef _DECISION_TREE_FEATURE_CLASSIFER_H_
#define _DECISION_TREE_FEATURE_CLASSIFER_H_

#include "BaseFeatureClassifier.h"

namespace duneML
{
	class DecisionTreeFeatureClassifier : public BaseFeatureClassifier
	{
	public:
		DecisionTreeFeatureClassifier()
		{
			meanNormalizer = 0.0;
			scaleNormalizer = 1.0;
		}
		~DecisionTreeFeatureClassifier() {}

		TrainingResults Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples);

		float Predict(const cv::Mat& example, bool normalize = true);

	private:
		//CvRTrees rt;
		cv::Ptr<cv::ml::DTrees> dt;
	};
}

#endif