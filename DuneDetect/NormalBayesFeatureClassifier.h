#ifndef _NORMAL_BAYES_FEATURE_CLASSIFER_H_
#define _NORMAL_BAYES_FEATURE_CLASSIFER_H_

#include "BaseFeatureClassifier.h"

namespace duneML
{
	class NormalBayesFeatureClassifier : public BaseFeatureClassifier
	{
	public:
		NormalBayesFeatureClassifier()
		{
			meanNormalizer = 0.0;
			scaleNormalizer = 1.0;
		}
		~NormalBayesFeatureClassifier() {}

		TrainingResults Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples);

		float Predict(const cv::Mat& example, bool normalize=true);

	private:
		//CvNormalBayesClassifier nb;
		cv::Ptr<cv::ml::NormalBayesClassifier> nbc;
	};
}

#endif