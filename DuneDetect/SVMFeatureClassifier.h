#ifndef _SVM_FEATURE_CLASSIFER_H_
#define _SVM_FEATURE_CLASSIFER_H_

#include "BaseFeatureClassifier.h"
#include <ml.h>

namespace duneML
{
	class SVMFeatureClassifier : public BaseFeatureClassifier
	{
	public:
		SVMFeatureClassifier() 
		{
			meanNormalizer = 0.0;
			scaleNormalizer = 1.0;
		}
		~SVMFeatureClassifier() {}

		TrainingResults Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples);

		float Predict(const cv::Mat& example);

	private:
		//CvSVM svm;
		cv::Ptr<cv::ml::SVM> svm;
	};
}

#endif