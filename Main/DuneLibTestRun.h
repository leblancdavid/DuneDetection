#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>

const std::string DUNE_DATASET_BASE_PATH = "E:/Projects/Thesis/DuneDetection/DuneDataset/";

#include "BaseDuneDetectorBenchmark.h"
#include "EdgeBasedDuneDetector.h"
#include "WatershedDuneDetector.h"
#include "DuneSegmentDetector.h"

void ComputePRCurvesForR(std::string &fileName, double minR, double maxR);
void ComputePRCurvesForK(std::string &fileName,
	std::string &imageFile,
	std::string &groundTruthFile,
	double minR,
	double maxR,
	int minK,
	int maxK);

void ProcessDataset(std::string &imageFile, std::string &groundTruthFile, dune::BaseDuneDetector *duneDetector)
{
	dune::BaseDuneDetectorBenchmark benchmark;
	benchmark.BenchmarkParams.MinError = 5.0;
	cv::Mat testImg = cv::imread(imageFile, 0);
	cv::Mat groundTruthImg = cv::imread(groundTruthFile, 0);
	std::cout << "Processing " + imageFile + "...";
	std::vector<dune::DuneSegment> segments = duneDetector->Extract(testImg);
	std::cout << "Done..." << std::endl; 
	dune::BenchmarkResults benchmarkResults = benchmark.GetResults(testImg, groundTruthImg, segments, imageFile + "_results_m1.jpg");

	std::cout << "Benchmark results: " << std::endl
		<< "\tTP: " << benchmarkResults.TP << std::endl
		<< "\tFP: " << benchmarkResults.FP << std::endl
		/*<< "\tTN: " << benchmarkResults.TN << std::endl
		<< "\tFN: " << benchmarkResults.FN << std::endl*/
		<< "\tPrecision: " << benchmarkResults.GetPrecision() << std::endl
		<< "\tRecall: " << benchmarkResults.GetRecall() << std::endl;
}

void RunDuneLibTest()
{
	//time_t rawtime;
	//time(&rawtime);
	//std::string timestamp = ctime(&rawtime);
	//timestamp.erase(timestamp.end() - 1, timestamp.end());
	//std::replace(timestamp.begin(), timestamp.end(), ':', '-');
	//timestamp.erase(std::remove(timestamp.begin(), timestamp.end(), ' '), timestamp.end());
	////std::string filename = DUNE_DATASET_BASE_PATH + "Results/EdgeBased - " + timestamp + ".txt";
	////std::string prFile = DUNE_DATASET_BASE_PATH + "Results/EdgeBased_PR - " + timestamp + ".txt";
	//std::string prFile = DUNE_DATASET_BASE_PATH + "Results/EdgeBased_PR_K - " + timestamp + ".txt";
	////ComputePRCurvesForR(prFile, -1.2, 2.0);

	//std::string imageFile = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg";
	//std::string groundTruthFile = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	//ComputePRCurvesForK(prFile,
	//	imageFile,
	//	groundTruthFile,
	//	-1.2,
	//	2.0,
	//	5,
	//	21);

	std::vector<std::string> trainingImages(1), trainingGroundTruth(1), testImages(1), testGroundTruth(1);

	dune::EdgeBasedDetectorParameters *parameters = new dune::EdgeBasedDetectorParameters();
	parameters->R = -0.2;
	dune::BaseDuneDetector *duneDetector = new dune::EdgeBasedDuneDetector(new dune::EdgeDetectorImageProcessor(), parameters);
	//dune::WatershedDuneDetectorParameters *parameters = new dune::WatershedDuneDetectorParameters();
	//dune::BaseDuneDetector *duneDetector = new dune::WatershedDuneDetector(new dune::WatershedImageProcessor(new dune::WatershedProcessorParameters()), parameters);
	//dune::BaseDuneDetector *duneDetector = new dune::DuneSegmentDetector(new dune::AdaptiveImageProcessor(new dune::AdaptiveImageProcParams()), new dune::DuneSegmentDetectorParameters());
	
	trainingImages[0] = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg";
	trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	ProcessDataset(trainingImages[0], trainingGroundTruth[0], duneDetector);
	testImages[0] = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test.jpg";
	testGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 test_gt.png";
	ProcessDataset(testImages[0], testGroundTruth[0], duneDetector);

	trainingImages[0] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
	ProcessDataset(trainingImages[0], trainingGroundTruth[0], duneDetector);
	testImages[0] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 test.jpg";
	testGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 test gt.png";
	ProcessDataset(testImages[0], testGroundTruth[0], duneDetector);

	trainingImages[0] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
	ProcessDataset(trainingImages[0], trainingGroundTruth[0], duneDetector);
	testImages[0] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test.jpg";
	testGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1 test_gt.png";
	ProcessDataset(testImages[0], testGroundTruth[0], duneDetector);

	trainingImages[0] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
	ProcessDataset(trainingImages[0], trainingGroundTruth[0], duneDetector);
	testImages[0] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg";
	testGroundTruth[0] = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test_gt.png";
	ProcessDataset(testImages[0], testGroundTruth[0], duneDetector);

	trainingImages[0] = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
	ProcessDataset(trainingImages[0], trainingGroundTruth[0], duneDetector);
	testImages[0] = DUNE_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg";
	testGroundTruth[0] = DUNE_DATASET_BASE_PATH + "WDC 1/WCD-1 test_gt.png";
	ProcessDataset(testImages[0], testGroundTruth[0], duneDetector);

	trainingImages[0] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	trainingGroundTruth[0] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
	ProcessDataset(trainingImages[0], trainingGroundTruth[0], duneDetector);
	testImages[0] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test.jpg";
	testGroundTruth[0] = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands 2 test-gt.png";
	ProcessDataset(testImages[0], testGroundTruth[0], duneDetector);
}

void ComputePRCurvesForR(std::string &fileName, double minR, double maxR)
{
	std::ofstream resultsFile(fileName);
	if (!resultsFile.good())
		return;

	//varying R.
	for (double r = minR; r <= maxR; r += 0.1)
	{
		//resultsFile << "TEST";
		//resultsFile.close();
		dune::EdgeBasedDetectorParameters *parameters = new dune::EdgeBasedDetectorParameters();
		parameters->R = r;
		dune::BaseDuneDetector *duneDetector = new dune::EdgeBasedDuneDetector(new dune::EdgeDetectorImageProcessor(), parameters);
		dune::BaseDuneDetectorBenchmark benchmark(duneDetector, dune::BenchmarkTestParameters(10.0));

		dune::BenchmarkResults results;
		std::string imageFile, groundTruthFile;

		imageFile = DUNE_DATASET_BASE_PATH;
		imageFile += "Kalahari 3/Kalahari 3 image.jpg";
		groundTruthFile = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
		std::cout << "Processing " + imageFile + "...";
		results = benchmark.GetResults(imageFile, groundTruthFile);
		std::cout << " Done" << std::endl;
		resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << "\t";
		//resultsFile << "Kalahari\t" << results.TP << "\t" << results.FP << std::endl;

		imageFile = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
		groundTruthFile = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
		std::cout << "Processing " + imageFile + "...";
		results = benchmark.GetResults(imageFile, groundTruthFile);
		std::cout << " Done" << std::endl;
		resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << "\t";
		//resultsFile << "Namib\t" << results.TP << "\t" << results.FP << std::endl;

		imageFile = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
		groundTruthFile = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
		std::cout << "Processing " + imageFile + "...";
		results = benchmark.GetResults(imageFile, groundTruthFile);
		std::cout << " Done" << std::endl;
		resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << "\t";
		//resultsFile << "Simpson\t" << results.TP << "\t" << results.FP << std::endl;

		imageFile = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
		groundTruthFile = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
		results = benchmark.GetResults(imageFile, groundTruthFile);
		std::cout << " Done" << std::endl;
		resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << "\t";
		//resultsFile << "Skeleton_Coast\t" << results.TP << "\t" << results.FP << std::endl;

		imageFile = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
		groundTruthFile = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
		std::cout << "Processing " + imageFile + "...";
		results = benchmark.GetResults(imageFile, groundTruthFile);
		std::cout << " Done" << std::endl;
		resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << "\t";
		//resultsFile << "WDC\t" << results.TP << "\t" << results.FP << std::endl;

		imageFile = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
		groundTruthFile = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
		std::cout << "Processing " + imageFile + "...";
		results = benchmark.GetResults(imageFile, groundTruthFile);
		std::cout << " Done" << std::endl;
		resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << std::endl;
		//resultsFile << "White_Sands\t" << results.TP << "\t" << results.FP << std::endl;

		//clean up
		//delete duneDetector;
		//delete parameters;
	}

	resultsFile.close();
}

void ComputePRCurvesForK(std::string &fileName,
	std::string &imageFile,
	std::string &groundTruthFile,
	double minR,
	double maxR,
	int minK,
	int maxK)
{
	std::ofstream resultsFile(fileName);
	if (!resultsFile.good())
		return;

	//varying R.
	for (double r = minR; r <= maxR; r += 0.1)
	{
		for (int k = minK; k <= maxK; k += 2)
		{
			dune::EdgeBasedDetectorParameters *parameters = new dune::EdgeBasedDetectorParameters();
			parameters->R = r;
			parameters->K = k;
			dune::BaseDuneDetector *duneDetector = new dune::EdgeBasedDuneDetector(new dune::EdgeDetectorImageProcessor(), parameters);
			dune::BaseDuneDetectorBenchmark benchmark(duneDetector, dune::BenchmarkTestParameters(10.0));

			dune::BenchmarkResults results;
			std::cout << "Processing R: " << r << ", K: " << k << " for " + imageFile + "...";
			results = benchmark.GetResults(imageFile, groundTruthFile);
			std::cout << " Done" << std::endl;
			resultsFile << results.GetPrecision() << "\t" << results.GetRecall() << "\t";
		}
		resultsFile << std::endl;
	}

	resultsFile.close();
}