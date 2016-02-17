#ifndef _GRADIENT_BOOSTED_TREE_FEATURE_CLASSIFER_H_
#define _GRADIENT_BOOSTED_TREE_FEATURE_CLASSIFER_H_

#include "BaseFeatureClassifier.h"
#include <ml.h>

namespace duneML
{
	class GradientBoostedTreeFeatureClassifier : public BaseFeatureClassifier
	{
	public:
		GradientBoostedTreeFeatureClassifier()
		{
			meanNormalizer = 0.0;
			scaleNormalizer = 1.0;
		}
		~GradientBoostedTreeFeatureClassifier() {}

		TrainingResults Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples);

		float Predict(const cv::Mat& example);

	private:
		CvGBTrees gbt;
	};
}

#endif