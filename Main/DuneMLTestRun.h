#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "DuneMLDatasetExtractor.h"
#include "DuneMLData.h"
#include "DuneSIFTFeatureExtractor.h"
#include "DuneOrbFeatureExtractor.h"
#include "DuneSURFFeatureExtractor.h"
#include "DuneSimplePixelFeatureExtractor.h"
#include "DuneDirectionalFeatureExtractor.h"
#include "SVMFeatureClassifier.h"
#include "NormalBayesFeatureClassifier.h"
#include "RandomTreeFeatureClassifier.h"
#include "DecisionTreesFeatureClassifier.h"
#include "GradientBoostedTreeFeatureClassifier.h"
#include "BaseDuneDetectorBenchmark.h"
#include "DuneRegionThinner.h"
#include "SkeletonizationRegionThinner.h"
#include "GenericImageProcessing.h"
#include "LaplacianScalePyramid.h"
#include "GaussianScalePyramid.h"
#include "LogFileHelper.h"
#include "SuperDuperImageProcessor.h"
#include "EdgeDirectionDuneDetector.h"
#include "MLDuneFiltering.h"

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
	int k = 5;
	cv::medianBlur(input, processed, k);
	cv::GaussianBlur(processed, output, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);
	//cv::equalizeHist(processed, output);
	//output = input;

	//cv::GaussianBlur(processed, processed, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);
	//cv::equalizeHist(processed, processed);
	//dune::imgproc::LocalScaleIntegralIlluminationNormalization(processed, output);

	//cv::GaussianBlur(processed, output, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);
	//cv::equalizeHist(output, output);
	//GaussianScalePyramid gsp;
	//gsp.Process(processed);
	//cv::Mat scaleImage = gsp.GetScaleImage();

	
	//cv::equalizeHist(processed, output);
	//cv::Mat sobel;
	//dune::imgproc::GetSobelImage(processed, sobel, 9);
	//dune::imgproc::IntegralIlluminationNormalization(input, output, 5);
	//dune::imgproc::LocalScaleIntegralIlluminationNormalization(sobel, output);

	//dune::EdgeDirectionImageProcessor ip;
	//dune::SuperDuperImageProcessor ip;
	//ip.Process(input, output);

	cv::imshow("preprocess", output);
	cv::waitKey(33);
	//cv::imwrite("image_processed.jpg", output);
}

void ExtractFeatureSet(const std::vector<duneML::DuneMLData> data,
	cv::Mat &positiveMat,
	cv::Mat &negativeMat,
	int numPosExamples,
	int numNegExamples,
	int bufferDistance)
{
	std::vector<std::vector<float>> featuresSetPos, featuresSetNeg;
	for (size_t i = 0; i < data.size(); ++i)
	{
		logFile << "Reading image " << data[i].ImageFileName << "..." << std::endl;
		cv::Mat image = cv::imread(data[i].ImageFileName, 0);
		
		cv::Mat processedImage;
		PreProcessImage(image, processedImage);

		logFile << "Reading image " << data[i].GroundTruthFileName << "..." << std::endl;
		cv::Mat gtImg = cv::imread(data[i].GroundTruthFileName, 0);

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

		GaussianScalePyramid gsp;
		////LaplacianScalePyramid lsp;
		//gsp.Process(processedImage);
		//cv::Mat scaleMap = gsp.GetScaleMap();

		//cv::Scalar meanScale, stdevScale;
		//cv::meanStdDev(scaleMap, meanScale, stdevScale);

		//std::cout << "Mean Scale Sigma: " << meanScale[0] << std::endl;
		//std::cout << "Stdev Scale Sigma: " << stdevScale[0] << std::endl;
		cv::Mat positiveDescriptors, negativeDescriptors;
		duneML::SIFTParameters siftParams;
		siftParams.FixedOrientation = false;
		siftParams.Sigma = 2.5;// meanScale[0] - stdevScale[0];
		siftParams.Orientation = data[i].SunOrientation;
		duneML::DuneSIFTFeatureExtractor features(siftParams);
		//duneML::DuneDirectionFeatureExtractor features;
		//duneML::DuneORBFeatureExtractor features;
		//duneML::DuneSURFFeatureExtractor features;
		//duneML::DuneSimplePixelFeatureExtractor features;
		logFile << "Computing features from data points..." << std::endl;

		features.Process(processedImage, positiveExamples, positiveDescriptors);
		features.Process(processedImage, negativeExamples, negativeDescriptors);

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

void ValidateImageClassifier(duneML::DuneMLData data,
	duneML::BaseFeatureClassifier *classifier,
	int bufferDistance)
{
	logFile << "Reading image " << data.ImageFileName << "..." << std::endl;
	cv::Mat image = cv::imread(data.ImageFileName, 0);

	cv::Mat processedImage;
	PreProcessImage(image, processedImage);

	logFile << "Reading image " << data.GroundTruthFileName << "..." << std::endl;
	cv::Mat gtImg = cv::imread(data.GroundTruthFileName, 0);

	cv::Mat resultImg(processedImage.rows, processedImage.cols, CV_8U);
	cv::cvtColor(processedImage, resultImg, CV_GRAY2BGR);
	cv::Mat binaryImg = processedImage.clone();
	binaryImg = cv::Scalar(0);
	cv::Mat responseImg = cv::Mat(processedImage.rows, processedImage.cols, CV_32F);
	responseImg = cv::Scalar(-1.0f);
	std::vector<dune::DuneSegment> segments(1);
	std::vector<dune::DuneSegmentData> segmentData;

	//GaussianScalePyramid gsp;
	////LaplacianScalePyramid lsp;
	//gsp.Process(processedImage);
	//cv::Mat scaleMap = gsp.GetScaleMap();

	//cv::Scalar meanScale, stdevScale;
	//cv::meanStdDev(scaleMap, meanScale, stdevScale);

	//int border = 0;
	duneML::SIFTParameters siftParams;
	siftParams.FixedOrientation = false;
	siftParams.Orientation = data.SunOrientation;
	siftParams.Sigma = 2.5;// meanScale[0] - stdevScale[0];
	duneML::DuneSIFTFeatureExtractor features(siftParams);
	//duneML::DuneDirectionFeatureExtractor features;
	////duneML::DuneORBFeatureExtractor features;
	////duneML::DuneSURFFeatureExtractor features;
	//duneML::DuneSimplePixelFeatureExtractor features;

	//GaussianScalePyramid gsp;
	////LaplacianScalePyramid lsp;
	//gsp.Process(processedImage);
	//cv::Mat scaleMap = gsp.GetScaleMap();
	//for (int y = border; y < processedImage.rows - border; ++y)
	//{
	//	std::vector<cv::KeyPoint> allPoints;
	//	cv::Mat allDescriptors;

	//	for (int x = border; x < processedImage.cols - border; ++x)
	//	{
	//		cv::KeyPoint keypoint;
	//		keypoint.pt = cv::Point(x, y);
	//		allPoints.push_back(keypoint);
	//	}

	//	features.Process(processedImage, allPoints, allDescriptors, scaleMap);
	//	for (size_t i = 0; i < allPoints.size(); ++i)
	//	{
	//		float val = classifier->Predict(allDescriptors.row(i));
	//		//std::cout << val << std::endl;
	//		responseImg.at<float>(allPoints[i].pt) = val;
	//		if (val > 0.0f)
	//		{
	//			resultImg.at<cv::Vec3b>(allPoints[i].pt)[0] = 255;
	//			resultImg.at<cv::Vec3b>(allPoints[i].pt)[1] /= 2;
	//			resultImg.at<cv::Vec3b>(allPoints[i].pt)[2] /= 2;

	//			segmentData.push_back(dune::DuneSegmentData(cv::Point(allPoints[i].pt.x, allPoints[i].pt.y), 0.0));
	//		}
	//		binaryImg.at<uchar>(allPoints[i].pt) = (val + 1.0f) * 127;
	//	}
	//}

	////duneML::DuneRegionThinner thinner;
	//duneML::SkeletonizationRegionThinner thinner;
	//cv::Mat thinResults = thinner.Thin(responseImg);
	//segments = thinner.GetDuneSegments(responseImg, data.ImageFileName + "_results_postproc");

	//cv::imshow("thinResults", thinResults);
	//cv::waitKey(0);

	std::string outputImgFile = data.ImageFileName + "_results_response.jpg";
	//segments[0].SetSegmentData(segmentData);
	cv::imwrite(outputImgFile, binaryImg);

	logFile << "Validating image..." << std::endl;

	dune::EdgeDirectionDuneDetector detector(
		new dune::EdgeDirectionImageProcessor(new dune::EdgeDirectionProcParams(9, 16, true, data.SunOrientation)),
		new dune::EdgeDirectionDetectorParameters());

	segments = detector.Extract(processedImage);
	duneML::MLDuneFiltering mlFilter;
	mlFilter.SetFeatureClassifier(classifier);
	mlFilter.SetFeatureDetector(&features);
	mlFilter.SetMinSegmentLength(50);
	mlFilter.SetThreshold(0.5);

	//std::vector<dune::DuneSegment> classifiedSegments = mlFilter.Filter(processedImage, segments);
	std::vector<dune::DuneSegment> classifiedSegments = mlFilter.FilterByResponse(processedImage, segments, data.SunOrientation);
	classifiedSegments = mlFilter.RemoveOverlappingSegments(processedImage, classifiedSegments, 10);

	std::ofstream resultsDatFile(data.ImageFileName + "_results_.dat");
	for (int i = 2; i <= 50; i += 2)
	{
		dune::BaseDuneDetectorBenchmark benchmark;
		benchmark.BenchmarkParams.MinError = i;
		dune::BenchmarkResults benchmarkResults = benchmark.GetResults(image, gtImg, classifiedSegments, data.ImageFileName + "_results_.png");
		resultsDatFile << i << '\t' << benchmarkResults.GetPrecision() << '\t' << benchmarkResults.GetRecall() << std::endl;
	}
	resultsDatFile.close();
	
	dune::BaseDuneDetectorBenchmark benchmark;
	benchmark.BenchmarkParams.MinError = 10;
	dune::BenchmarkResults benchmarkResults = benchmark.GetResults(image, gtImg, classifiedSegments, data.ImageFileName + "_results_.png");
	logFile << "Benchmark results: " << std::endl
		<< "\tTP: " << benchmarkResults.TP << std::endl
		<< "\tFP: " << benchmarkResults.FP << std::endl
		/*<< "\tTN: " << benchmarkResults.TN << std::endl
		<< "\tFN: " << benchmarkResults.FN << std::endl*/
		<< "\tPrecision: " << benchmarkResults.GetPrecision() << std::endl
		<< "\tRecall: " << benchmarkResults.GetRecall() << std::endl
		<< "\tAngular Error: " << benchmarkResults.AngularError << std::endl
		<< "\tSpacing Error: " << benchmarkResults.SpacingError << std::endl
		<< "\tNormalized Spacing Error: " << benchmarkResults.NormSpacingError << std::endl;
}


void RunMLTest(const std::vector<duneML::DuneMLData> trainingData,
	const std::vector<duneML::DuneMLData> testData,
	int numPosExamples,
	int numNegExamples,
	int bufferDistance)
{
	std::vector<cv::KeyPoint> trainPos, testPos, trainNeg, testNeg;
	cv::Mat posTrain, negTrain, posTest, negTest;

	ExtractFeatureSet(trainingData, posTrain, negTrain, numPosExamples, numNegExamples, bufferDistance / 2);
	ExtractFeatureSet(testData, posTest, negTest, numPosExamples, numNegExamples, bufferDistance / 2);

	logFile << "Training Classifier..." << std::endl;
	//duneML::BaseFeatureClassifier *classifier = new duneML::SVMFeatureClassifier();
	//duneML::NormalBayesFeatureClassifier *classifier = new duneML::NormalBayesFeatureClassifier();
	duneML::BaseFeatureClassifier *classifier = new duneML::RandomTreeFeatureClassifier();
	//duneML::BaseFeatureClassifier *classifier = new duneML::DecisionTreeFeatureClassifier();
	//duneML::GradientBoostedTreeFeatureClassifier classifier;
	duneML::TrainingResults results = classifier->Train(posTrain, negTrain);

	logFile << "Training set results: " << std::endl
		<< "\tTPR: " << results.TruePositiveRate << std::endl
		<< "\tFPR: " << results.FalsePositiveRate << std::endl;
		//<< "\tTNR: " << results.TrueNegativeRate << std::endl
		//<< "\tFNR: " << results.FalseNegativeRate << std::endl;

	duneML::TrainingResults testResults;

	logFile << "Validating results for test feature set..." << std::endl;
	for (int row = 0; row < posTest.rows; ++row)
	{
		float val = classifier->Predict(posTest.row(row));
		//std::cout << val << std::endl;
		if (val > 0.0f)
			testResults.TruePositiveRate++;
		else
			testResults.FalseNegativeRate++;
	}

	for (int row = 0; row < negTest.rows; ++row)
	{
		float val = classifier->Predict(negTest.row(row));
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
	for (size_t i = 0; i < trainingData.size(); ++i)
	{
		ValidateImageClassifier(trainingData[i], classifier, bufferDistance);
	}

	logFile << "Validating test images..." << std::endl;
	for (size_t i = 0; i < testData.size(); ++i)
	{
		ValidateImageClassifier(testData[i], classifier, bufferDistance);
	}
} 

void RunMLTest()
{
	logFile = GetTimeStampedFileStream(DUNE_ML_DATASET_BASE_PATH + "Results/", "log_file");

	//std::vector<duneML::DuneMLData> trainingData(1), testData(1);
	//logFile.open(DUNE_ML_DATASET_BASE_PATH + "Results/log_file.txt");

	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	//trainingData[0].SunOrientation = -0.2979;
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test_gt.png";
	//testData[0].SunOrientation = -0.4595;
	//RunMLTest(trainingData, testData, 1000, 1000, 10);

	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
	//trainingData[0].SunOrientation = -0.5461;
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test gt.png";
	//testData[0].SunOrientation = -0.5782;
	//RunMLTest(trainingData, testData, 1000, 1000, 10);

	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
	//trainingData[0].SunOrientation = -0.4859;
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test_gt.png";
	//testData[0].SunOrientation = -0.5086;
	//RunMLTest(trainingData, testData, 1000, 1000, 10);

	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
	//trainingData[0].SunOrientation = -0.6931;
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test_gt.png";
	//testData[0].SunOrientation = -0.7182;
	//RunMLTest(trainingData, testData, 1000, 1000, 10);

	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
	//trainingData[0].SunOrientation = 0.3285 + 3.1416;//-2.8131;//0.3285;
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test_gt.png";
	//testData[0].SunOrientation = 0.3041 + 3.1416;//-2.8375;//0.3041;
	//RunMLTest(trainingData, testData, 1000, 1000, 10);

	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
	//trainingData[0].SunOrientation = -0.3755 + 3.1416;; //approx
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test-gt.png";
	//testData[0].SunOrientation = -0.4875 + 3.1416;
	//RunMLTest(trainingData, testData, 1000, 1000, 10);

	std::vector<duneML::DuneMLData> trainingData(7), testData(7);

	trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1.png";
	trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1 gt.png";
	trainingData[0].SunOrientation = 0.3378;//-2.8131;//0.3285;
	trainingData[1].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 3.png";
	trainingData[1].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 3 gt.png";
	trainingData[1].SunOrientation = 0.5090;//-2.8131;//0.3285;
	trainingData[2].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 5.png";
	trainingData[2].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 5 gt.png";
	trainingData[2].SunOrientation = 0.1809;//-2.8131;//0.3285;
	trainingData[3].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 7.png";
	trainingData[3].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 7 gt.png";
	trainingData[3].SunOrientation = 0.3534;//-2.8131;//0.3285;
	trainingData[4].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 9.png";
	trainingData[4].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 9 gt.png";
	trainingData[4].SunOrientation = 0.2653;//-2.8131;//0.3285;
	trainingData[5].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 11.png";
	trainingData[5].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 11 gt.png";
	trainingData[5].SunOrientation = 0.3375;//-2.8131;//0.3285;
	trainingData[6].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 13.png";
	trainingData[6].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 13 gt.png";
	trainingData[6].SunOrientation = -0.6765;//-2.8131;//0.3285;


	testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 2.png";
	testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 2 gt.png";
	testData[0].SunOrientation = 0.2847;//-2.8375;//0.3041;
	testData[1].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 4.png";
	testData[1].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 4 gt.png";
	testData[1].SunOrientation = 0.3312;//-2.8375;//0.3041;
	testData[2].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 6.png";
	testData[2].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 6 gt.png";
	testData[2].SunOrientation = -0.0890;//-2.8375;//0.3041;
	testData[3].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 8.png";
	testData[3].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 8 gt.png";
	testData[3].SunOrientation = 0.1898;//-2.8375;//0.3041;
	testData[4].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 10.png";
	testData[4].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 10 gt.png";
	testData[4].SunOrientation = 0.4829;//-2.8375;//0.3041;
	testData[5].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 12.png";
	testData[5].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 12 gt.png";
	testData[5].SunOrientation = 0.3302;//-2.8375;//0.3041;
	testData[6].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 15.png";
	testData[6].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 15 gt.png";
	testData[6].SunOrientation = -0.6973 + 3.1416;//-2.8375;//0.3041;

	RunMLTest(trainingData, testData, 1000, 1000, 10);


















	//std::vector<duneML::DuneMLData> trainingData(6), testData(6);
	//trainingData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg";
	//trainingData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	//trainingData[0].SunOrientation = -0.2979;
	//testData[0].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test.jpg";
	//testData[0].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test_gt.png";
	//testData[0].SunOrientation = -0.4595;

	//trainingData[1].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	//trainingData[1].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
	//trainingData[1].SunOrientation = -0.5461;
	//testData[1].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test.jpg";
	//testData[1].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Namib 2/Namib 2 test gt.png";
	//testData[1].SunOrientation = -0.5782;

	//trainingData[2].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	//trainingData[2].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
	//trainingData[2].SunOrientation = -0.4859;
	//testData[2].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test.jpg";
	//testData[2].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test_gt.png";
	//testData[2].SunOrientation = -0.5086;

	//trainingData[3].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	//trainingData[3].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
	//trainingData[3].SunOrientation = -0.6931;
	//testData[3].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg";
	//testData[3].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test_gt.png";
	//testData[3].SunOrientation = -0.7182;

	//trainingData[4].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	//trainingData[4].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
	//trainingData[4].SunOrientation = 0.3285 + 3.1416;;//-2.8131;//0.3285;
	//testData[4].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg";
	//testData[4].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test_gt.png";
	//testData[4].SunOrientation = 0.3041 + 3.1416;;//-2.8375;//0.3041;

	//trainingData[5].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	//trainingData[5].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
	//trainingData[5].SunOrientation = -0.3755; //approx
	//testData[5].ImageFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test.jpg";
	//testData[5].GroundTruthFileName = DUNE_ML_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test-gt.png";
	//testData[5].SunOrientation = -0.4875;
	//RunMLTest(trainingData, testData, 5000, 5000, 10);	
}