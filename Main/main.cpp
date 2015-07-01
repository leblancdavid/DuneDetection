#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>

const std::string DUNE_DATASET_BASE_PATH = "E:/Projects/Thesis/DuneDetection/DuneDataset/";

#include "DuneSegmentDetectorBenchmark.h"
#include "EdgeBasedDetectorBenchmark.h"
#include "WatershedDuneDetectorBenchmark.h"
#include "FrequencySpaceDetectorBenchmark.h"

int main()
{
	time_t rawtime;
	time(&rawtime);
	std::string timestamp = ctime(&rawtime);
	timestamp.erase(timestamp.end() - 1, timestamp.end());
	std::replace(timestamp.begin(), timestamp.end(), ':', '-');
	timestamp.erase(std::remove(timestamp.begin(), timestamp.end(), ' '), timestamp.end());
	std::string filename = DUNE_DATASET_BASE_PATH + "Results/EdgeBased - " + timestamp + ".txt";
	std::ofstream resultsFile(filename);

	if (!resultsFile.good())
		return -1;
	//resultsFile << "TEST";
	//resultsFile.close();

	//dune::DuneSegmentDetectorBenchmark benchmark = dune::DuneSegmentDetectorBenchmark();
	dune::EdgeBasedDetectorBenchmark benchmark = dune::EdgeBasedDetectorBenchmark();
	//dune::WatershedDuneDetectorBenchmark benchmark = dune::WatershedDuneDetectorBenchmark();
	//dune::FrequencySpaceDetectorBenchmark benchmark = dune::FrequencySpaceDetectorBenchmark();

	benchmark.BenchmarkParams = dune::BenchmarkTestParameters(10.0);
	dune::BenchmarkResults results;
	std::string imageFile, groundTruthFile;

	imageFile = DUNE_DATASET_BASE_PATH;
	imageFile += "Kalahari 3/Kalahari 3 image.jpg";
	groundTruthFile = DUNE_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3_gt.bmp";
	results = benchmark.GetResults(imageFile, groundTruthFile);
	resultsFile << "Kalahari\t" << results.TP << "\t" << results.FP << std::endl;


	imageFile = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	groundTruthFile = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2_gt.bmp";
	results = benchmark.GetResults(imageFile, groundTruthFile);
	resultsFile << "Namib\t" << results.TP << "\t" << results.FP << std::endl;

	imageFile = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	groundTruthFile = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_gt.bmp";
	results = benchmark.GetResults(imageFile, groundTruthFile);
	resultsFile << "Simpson\t" << results.TP << "\t" << results.FP << std::endl;

	imageFile = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	groundTruthFile = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_gt.bmp";
	results = benchmark.GetResults(imageFile, groundTruthFile);
	resultsFile << "Skeleton_Coast\t" << results.TP << "\t" << results.FP << std::endl;

	imageFile = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	groundTruthFile = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_gt.bmp";
	results = benchmark.GetResults(imageFile, groundTruthFile);
	resultsFile << "WDC\t" << results.TP << "\t" << results.FP << std::endl;

	imageFile = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	groundTruthFile = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_gt.bmp";
	results = benchmark.GetResults(imageFile, groundTruthFile);
	resultsFile << "White_Sands\t" << results.TP << "\t" << results.FP << std::endl;

	resultsFile.close();

	//proc.AddImageFile(imageFile);
	////imageFile = DUNE_DATASET_BASE_PATH + "Kumtagh 1/Kumtagh 1 image.jpg";
	////proc.AddImageFile(imageFile);
	//imageFile = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	//proc.AddImageFile(imageFile);
	//imageFile = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	//proc.AddImageFile(imageFile);
	//imageFile = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	//proc.AddImageFile(imageFile);
	//imageFile = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	//proc.AddImageFile(imageFile);
	//imageFile = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	//proc.AddImageFile(imageFile);
	//proc.Process();

	return 0;
}
