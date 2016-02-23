#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "DuneMLDatasetExtractor.h"
#include "DuneSIFTFeatureExtractor.h"
#include "DuneOrbFeatureExtractor.h"
#include "DuneSURFFeatureExtractor.h"
#include "DuneSimplePixelFeatureExtractor.h"
#include "SVMFeatureClassifier.h"
#include "NormalBayesFeatureClassifier.h"
#include "RandomTreeFeatureClassifier.h"
#include "GradientBoostedTreeFeatureClassifier.h"
#include "BaseDuneDetectorBenchmark.h"
#include "DuneRegionThinner.h"
#include "SkeletonizationRegionThinner.h"
#include "GenericImageProcessing.h"
#include "LaplacianScalePyramid.h"
#include "GaussianScalePyramid.h"
#include "LogFileHelper.h"
#include "EdgeDirectionImageProcessor.h"
#include "SuperDuperImageProcessor.h"

const std::string DUNE_ML_DATASET_BASE_PATH = "E:/Projects/Thesis/DuneDetection/DuneDataset/";

std::ofstream logFile;

void SplitSet(const std::vector<cv::KeyPoint> &input, std::vector<cv::KeyPoint> &trainingPoints, std::vector<cv::KeyPoint> &testPoints, int width)
{
	int half = width / 2;
	trainingPoints.clear();
	testPoints.clear();
	for (size_t i = 0; i < input.size(); ++i)
	{
		if (input[i].pt.x < half)
			trainingPoints.push_back(input[i]);
		else
			testPoints.push_back(input[i]);
	}
}

void PreProcessImage(const cv::Mat &input, cv::Mat &output)
{
	//cv::imwrite("image_unprocessed.jpg", input);
	cv::Mat processed;
	cv::medianBlur(input, processed, 5);

	//GaussianScalePyramid gsp;
	//gsp.Process(processed);
	//cv::Mat scaleImage = gsp.GetScaleImage();

	//cv::GaussianBlur(scaleImage, output, cv::Size(5, 5), 1.5, 1.5);
	//cv::equalizeHist(processed, output);
	//cv::Mat sobel;
	//dune::imgproc::GetSobelImage(processed, sobel, 9);
	//dune::imgproc::IntegralIlluminationNormalization(input, output, 5);
	//dune::imgproc::LocalScaleIntegralIlluminationNormalization(sobel, output);

	dune::EdgeDirectionImageProcessor ip;
	//dune::SuperDuperImageProcessor ip;
	ip.Process(processed, output);

	cv::imshow("preprocess", output);
	cv::waitKey(33);
	//cv::imwrite("image_processed.jpg", output);
}

void ExtractFeatureSet(const std::vector<std::string> &images,
	const std::vector<std::string> &groundTruth,
	cv::Mat &positiveMat,
	cv::Mat &negativeMat,
	int numPosExamples,
	int numNegExamples,
	int bufferDistance)
{
	std::vector<std::vector<float>> featuresSetPos, featuresSetNeg;
	for (size_t i = 0; i < images.size(); ++i)
	{
		logFile << "Reading image " << images[i] << "..." << std::endl;
		cv::Mat image = cv::imread(images[i], 0);
		
		cv::Mat processedImage;
		PreProcessImage(image, processedImage);

		logFile << "Reading image " << groundTruth[i] << "..." << std::endl;
		cv::Mat gtImg = cv::imread(groundTruth[i], 0);

		std::vector<cv::KeyPoint> positiveExamples, negativeExamples;
		duneML::DuneMLDatasetParameters params;
		params.NumNegativeExamples = numNegExamples;
		params.NumPositiveExamples = numPosExamples;
		params.BufferDistancePixels = bufferDistance;
		duneML::DuneMLDatasetExtractor extractor(params);
		logFile << "Extracting data points..." << std::endl;
		extractor.GetDatasetPoints(gtImg, positiveExamples, negativeExamples);

		//SplitSet(positiveExamples, trainPos, testPos, image.cols);
		//SplitSet(negativeExamples, trainNeg, testNeg, image.cols);

		cv::Mat positiveDescriptors, negativeDescriptors;
		//duneML::DuneSIFTFeatureExtractor features;
		//duneML::DuneORBFeatureExtractor features;
		//duneML::DuneSURFFeatureExtractor features;
		duneML::DuneSimplePixelFeatureExtractor features;
		logFile << "Computing features from data points..." << std::endl;

		GaussianScalePyramid gsp;
		//LaplacianScalePyramid lsp;
		gsp.Process(processedImage);
		cv::Mat scaleMap = gsp.GetScaleMap();
		features.Process(processedImage, positiveExamples, positiveDescriptors, scaleMap);
		features.Process(processedImage, negativeExamples, negativeDescriptors, scaleMap);

		for (int i = 0; i < positiveDescriptors.rows; ++i)
		{
			std::vector<float> descriptor(positiveDescriptors.cols);
			for (int j = 0; j < positiveDescriptors.cols; ++j)
			{
				descriptor[j] = positiveDescriptors.at<float>(i, j);
			}
			featuresSetPos.push_back(descriptor);
		}

		for (int i = 0; i < negativeDescriptors.rows; ++i)
		{
			std::vector<float> descriptor(negativeDescriptors.cols);
			for (int j = 0; j < negativeDescriptors.cols; ++j)
			{
				descriptor[j] = negativeDescriptors.at<float>(i, j);
			}
			featuresSetNeg.push_back(descriptor);
		}
	}

	positiveMat = cv::Mat(featuresSetPos.size(), featuresSetPos[0].size(), CV_32F);
	negativeMat = cv::Mat(featuresSetNeg.size(), featuresSetNeg[0].size(), CV_32F);

	for (size_t i = 0; i < featuresSetPos.size(); ++i)
	{
		for (size_t j = 0; j < featuresSetPos[i].size(); ++j)
		{
			positiveMat.at<float>(i, j) = featuresSetPos[i][j];
		}
	}

	for (size_t i = 0; i < featuresSetNeg.size(); ++i)
	{
		for (size_t j = 0; j < featuresSetNeg[i].size(); ++j)
		{
			negativeMat.at<float>(i, j) = featuresSetNeg[i][j];
		}
	}
}

void ValidateImageClassifier(const std::string &imageName,
	const std::string &gtImageName,
	duneML::BaseFeatureClassifier &classifier,
	int bufferDistance)
{
	logFile << "Reading image " << imageName << "..." << std::endl;
	cv::Mat image = cv::imread(imageName, 0);

	cv::Mat processedImage;
	PreProcessImage(image, processedImage);

	logFile << "Reading image " << gtImageName << "..." << std::endl;
	cv::Mat gtImg = cv::imread(gtImageName, 0);

	cv::Mat resultImg(processedImage.rows, processedImage.cols, CV_8U);
	cv::cvtColor(processedImage, resultImg, CV_GRAY2BGR);
	cv::Mat binaryImg = processedImage.clone();
	binaryImg = cv::Scalar(0);
	cv::Mat responseImg = cv::Mat(processedImage.rows, processedImage.cols, CV_32F);
	responseImg = cv::Scalar(-1.0f);
	std::vector<dune::DuneSegment> segments(1);
	std::vector<dune::DuneSegmentData> segmentData;

	int border = 0;
	//duneML::DuneSIFTFeatureExtractor features;
	//duneML::DuneORBFeatureExtractor features;
	//duneML::DuneSURFFeatureExtractor features;
	duneML::DuneSimplePixelFeatureExtractor features;

	GaussianScalePyramid gsp;
	//LaplacianScalePyramid lsp;
	gsp.Process(processedImage);
	cv::Mat scaleMap = gsp.GetScaleMap();
	for (int y = border; y < processedImage.rows - border; ++y)
	{
		std::vector<cv::KeyPoint> allPoints;
		cv::Mat allDescriptors;

		for (int x = border; x < processedImage.cols - border; ++x)
		{
			cv::KeyPoint keypoint;
			keypoint.pt = cv::Point(x, y);
			allPoints.push_back(keypoint);
		}

		features.Process(processedImage, allPoints, allDescriptors, scaleMap);
		for (size_t i = 0; i < allPoints.size(); ++i)
		{
			float val = classifier.Predict(allDescriptors.row(i));
			responseImg.at<float>(allPoints[i].pt) = val;
			if (val > 0.0f)
			{
				resultImg.at<cv::Vec3b>(allPoints[i].pt)[0] = 255;
				resultImg.at<cv::Vec3b>(allPoints[i].pt)[1] /= 2;
				resultImg.at<cv::Vec3b>(allPoints[i].pt)[2] /= 2;

				segmentData.push_back(dune::DuneSegmentData(cv::Point(allPoints[i].pt.x, allPoints[i].pt.y), 0.0));
			}
			binaryImg.at<uchar>(allPoints[i].pt) = (val + 1.0f) * 127;
		}
	}

	//duneML::DuneRegionThinner thinner;
	duneML::SkeletonizationRegionThinner thinner;
	//cv::Mat thinResults = thinner.Thin(responseImg);
	segments = thinner.GetDuneSegments(responseImg, imageName + "_postproc");

	//cv::imshow("thinResults", thinResults);
	//cv::waitKey(0);

	std::string outputImgFile = imageName + "_response.jpg";
	//segments[0].SetSegmentData(segmentData);
	cv::imwrite(outputImgFile, binaryImg);

	logFile << "Validating image..." << std::endl;
	dune::BaseDuneDetectorBenchmark benchmark;
	benchmark.BenchmarkParams.MinError = 10.0;
	dune::BenchmarkResults benchmarkResults = benchmark.GetResults(image, gtImg, segments, imageName + "_results.jpg");

	logFile << "Benchmark results: " << std::endl
		<< "\tTP: " << benchmarkResults.TP << std::endl
		<< "\tFP: " << benchmarkResults.FP << std::endl
		/*<< "\tTN: " << benchmarkResults.TN << std::endl
		<< "\tFN: " << benchmarkResults.FN << std::endl*/
		<< "\tPrecision: " << benchmarkResults.GetPrecision() << std::endl
		<< "\tRecall: " << benchmarkResults.GetRecall() << std::endl
		<< "\tAngular Error: " << benchmarkResults.AngularError * 180.0 / 3.1416 << std::endl
		<< "\tSpacing Error: " << benchmarkResults.SpacingError << std::endl
		<< "\tNormalized Spacing Error: " << benchmarkResults.NormSpacingError << std::endl;
}


void RunMLTest(const std::vector<std::string> &trainingImages, 
	const std::vector<std::string> &trainingGroundTruth,
	const std::vector<std::string> &testImages,
	const std::vector<std::string> &testGroundTruth,
	int numPosExamples,
	int numNegExamples,
	int bufferDistance)
{
	std::vector<cv::KeyPoint> trainPos, testPos, trainNeg, testNeg;
	cv::Mat posTrain, negTrain, posTest, negTest;

	ExtractFeatureSet(trainingImages, trainingGroundTruth, posTrain, negTrain, numPosExamples, numNegExamples, bufferDistance/2);
	ExtractFeatureSet(testImages, testGroundTruth, posTest, negTest, numPosExamples, numNegExamples, bufferDistance/2);

	logFile << "Training Classifier..." << std::endl;
	duneML::SVMFeatureClassifier classifier;
	//duneML::NormalBayesFeatureClassifier classifier;
	//duneML::RandomTreeFeatureClassifier classifier;
	//duneML::GradientBoostedTreeFeatureClassifier classifier;
	duneML::TrainingResults results = classifier.Train(posTrain, negTrain);

	logFile << "Training set results: " << std::endl
		<< "\tTPR: " << results.TruePositiveRate << std::endl
		<< "\tFPR: " << results.FalsePositiveRate << std::endl;
		//<< "\tTNR: " << results.TrueNegativeRate << std::endl
		//<< "\tFNR: " << results.FalseNegativeRate << std::endl;

	duneML::TrainingResults testResults;

	logFile << "Validating results for test feature set..." << std::endl;
	for (int row = 0; row < posTest.rows; ++row)
	{
		float val = classifier.Predict(posTest.row(row));
		if (val > 0.0f)
			testResults.TruePositiveRate++;
		else
			testResults.FalseNegativeRate++;
	}

	for (int row = 0; row < negTest.rows; ++row)
	{
		float val = classifier.Predict(negTest.row(row));
		if (val < 0.0f)
			testResults.TrueNegativeRate++;
		else
			testResults.FalsePositiveRate++;
	}

	testResults.FalseNegativeRate /= (double)posTest.rows;
	testResults.TruePositiveRate /= (double)posTest.rows;
	testResults.FalsePositiveRate /= (double)negTest.rows;
	testResults.TrueNegativeRate /= (double)negTest.rows;

	logFile << "Test set results: " << std::endl
		<< "\tTPR: " << testResults.TruePositiveRate << std::endl
		<< "\tFPR: " << testResults.FalsePositiveRate << std::endl;
		//<< "\tTNR: " << testResults.TrueNegativeRate << std::endl
		//<< "\tFNR: " << testResults.FalseNegativeRate << std::endl;


	//Ok just for the heck of it, test every position on the image.
	logFile << "Validating training images..." << std::endl;
	for (size_t i = 0; i < trainingImages.size(); ++i)
	{
		ValidateImageClassifier(trainingImages[i], trainingGroundTruth[i], classifier, bufferDistance);
	}

	logFile << "Validating test images..." << std::endl;
	for (size_t i = 0; i < testImages.size(); ++i)
	{
		ValidateImageClassifier(testImages[i], testGroundTruth[i], classifier, bufferDistance);
	}
} 

void RunMLTest()
{
	logFile = GetTimeStampedFileStream(DUNE_ML_DATASET_BASE_PATH + "Results/", "log_file");

	std::vector<std::string> trainingImages(1), trainingGroundTruth(1), testImages(1), testGroundTruth(1);
	//std::vector<double> trainingImageOrientations(1), testImagesOrientations(1);
	//logFile.open(DUNE_ML_DATASET_BASE_PATH + "Results/log_file.txt");

	trainingImages[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg";
	trainingGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	testImages[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test.jpg";
	testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test_gt.png";
	RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 10);

	//trainingImages[0] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	//trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
	//testImages[0] = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test.jpg";
	//testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test gt.png";
	//RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 10);

	//trainingImages[0] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	//trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
	//testImages[0] = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test.jpg";
	//testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test_gt.png";
	//RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 10);

	//trainingImages[0] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	//trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
	//testImages[0] = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg";
	//testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test_gt.png";
	//RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 10);

	//trainingImages[0] = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	//trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
	//testImages[0] = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg";
	//testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test_gt.png";
	//RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 10);

	//trainingImages[0] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	//trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
	//testImages[0] = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test.jpg";
	//testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test-gt.png";
	//RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 10);

	/*std::vector<std::string> trainingImages(6), trainingGroundTruth(6), testImages(6), testGroundTruth(6);
	trainingImages[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg";
	trainingGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	testImages[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test.jpg";
	testGroundTruth[0] = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test_gt.png";

	trainingImages[1] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	trainingGroundTruth[1] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
	testImages[1] = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test.jpg";
	testGroundTruth[1] = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test gt.png";

	trainingImages[2] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	trainingGroundTruth[2] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
	testImages[2] = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test.jpg";
	testGroundTruth[2] = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test_gt.png";

	trainingImages[3] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	trainingGroundTruth[3] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
	testImages[3] = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg";
	testGroundTruth[3] = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test_gt.png";

	trainingImages[4] = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	trainingGroundTruth[4] = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
	testImages[4] = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg";
	testGroundTruth[4] = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test_gt.png";

	trainingImages[5] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	trainingGroundTruth[5] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
	testImages[5] = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test.jpg";
	testGroundTruth[5] = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test-gt.png";

	RunMLTest(trainingImages, trainingGroundTruth, testImages, testGroundTruth, 5000, 5000, 5);*/
	
}