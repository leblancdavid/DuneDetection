#include "RandomTreeFeatureClassifier.h"

namespace duneML
{
	TrainingResults RandomTreeFeatureClassifier::Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples)
	{
		cv::Mat exampleSet(positiveExamples.rows + negativeExamples.rows, positiveExamples.cols, CV_32F);
		cv::Mat responsesSet(exampleSet.rows, 1, CV_32F);

		for (int row = 0; row < positiveExamples.rows; ++row)
		{
			for (int col = 0; col < exampleSet.cols; ++col)
			{
				exampleSet.at<float>(row, col) = positiveExamples.at<float>(row, col);
			}
			responsesSet.at<float>(row, 0) = 1.0f;
		}

		for (int row = positiveExamples.rows; row < exampleSet.rows; ++row)
		{
			for (int col = 0; col < exampleSet.cols; ++col)
			{
				exampleSet.at<float>(row, col) = negativeExamples.at<float>(row - positiveExamples.rows, col);
			}
			responsesSet.at<float>(row, 0) = -1.0f;
		}

		cv::Scalar mean, stdDev;
		cv::meanStdDev(exampleSet, mean, stdDev);

		meanNormalizer = mean[0];
		scaleNormalizer = stdDev[0] * 3.0;

		exampleSet = NormalizeMat(exampleSet);

		//rt.train(exampleSet, CV_ROW_SAMPLE, responsesSet);

		TrainingResults results;
		for (int row = 0; row < positiveExamples.rows; ++row)
		{
			float val = Predict(positiveExamples.row(row));
			if (val > 0.0f)
				results.TruePositiveRate++;
			else
				results.FalseNegativeRate++;
		}

		for (int row = 0; row < negativeExamples.rows; ++row)
		{
			float val = Predict(negativeExamples.row(row));
			if (val < 0.0f)
				results.TrueNegativeRate++;
			else
				results.FalsePositiveRate++;
		}

		results.FalseNegativeRate /= (double)positiveExamples.rows;
		results.TruePositiveRate /= (double)positiveExamples.rows;
		results.FalsePositiveRate /= (double)negativeExamples.rows;
		results.TrueNegativeRate /= (double)negativeExamples.rows;

		return results;
	}

	float RandomTreeFeatureClassifier::Predict(const cv::Mat& example)
	{
		return 0;
		//return rt.predict(NormalizeMat(example));
	}
}