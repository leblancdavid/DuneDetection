#include "SVMFeatureClassifier.h"

using namespace cv::ml;

namespace duneML
{
	TrainingResults SVMFeatureClassifier::Train(const cv::Mat& positiveExamples, const cv::Mat& negativeExamples)
	{
		normType = MINMAX;

		cv::Mat exampleSet(positiveExamples.rows + negativeExamples.rows, positiveExamples.cols, CV_32F);
		cv::Mat responsesSet(exampleSet.rows, 1, CV_32S);

		for (int row = 0; row < positiveExamples.rows; ++row)
		{
			for (int col = 0; col < exampleSet.cols; ++col)
			{
				exampleSet.at<float>(row, col) = positiveExamples.at<float>(row, col);
			}
			responsesSet.at<int>(row, 0) = 1;
		}

		for (int row = positiveExamples.rows; row < exampleSet.rows; ++row)
		{
			for (int col = 0; col < exampleSet.cols; ++col)
			{
				exampleSet.at<float>(row, col) = negativeExamples.at<float>(row - positiveExamples.rows, col);
			}
			responsesSet.at<int>(row, 0) = -1;
		}

		cv::Scalar mean, stdDev;
		cv::meanStdDev(exampleSet, mean, stdDev);

		meanNormalizer = mean[0];
		scaleNormalizer = stdDev[0] * 3.0;

		exampleSet = NormalizeMat(exampleSet);

		//CvSVMParams params;
 		/*params.svm_type = CvSVM::NU_SVC;
		params.kernel_type = CvSVM::RBF;
		params.degree = 2;
		params.nu = 0.5;
		svm.train_auto(exampleSet, responsesSet, cv::Mat(), cv::Mat(), params);*/
		cv::Ptr<cv::ml::TrainData> inputData = cv::ml::TrainData::create(exampleSet, cv::ml::ROW_SAMPLE, responsesSet);
		//svm = cv::ml::StatModel::train<cv::ml::SVM>(inputData);

		svm = SVM::create();
		svm->setType(SVM::NU_SVC);
		svm->setKernel(SVM::RBF);
		svm->setNu(0.5);
		svm->setDegree(2.0);
		svm->setP(0.5);
		svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
		svm->trainAuto(inputData);
		//svm->train(inputData);

		TrainingResults results;
		for (int row = 0; row < positiveExamples.rows; ++row)
		{
			float val = Predict(positiveExamples.row(row));
			std::cout << val << std::endl;
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

	float SVMFeatureClassifier::Predict(const cv::Mat& example, bool normalize)
	{
		//return 0.0;
		if (normalize)
			return svm->predict(NormalizeMat(example));
		else
			return svm->predict(example);
	}

}