#define _CRT_SECURE_NO_WARNINGS

#include "DuneLibTestRun.h"
#include "DuneMLTestRun.h"

#include "GaborFilter.h"
#include "DiscreteFourierTransform.h"
#include "GaborFilterImageEnhancement.h"
#include "DFTImageEnhancement.h"
#include "GenericImageProcessing.h"
#include "GaussianScalePyramid.h"
#include "LaplacianScalePyramid.h"
#include "EdgeDirectionImageProcessor.h"
#include "SuperDuperImageProcessor.h"
#include "ImageVectorDraw.h"

int main()
{

	//RunDuneLibTest();
	RunMLTest();

	//cv::Mat dx, dy;
	//cv::getDerivKernels(dx, dy, 2, 2, 7);
	////cv::getDerivKernels(dy, dy, 0, 2, 7);
	//dx = -1.0 * dx;
	//dy = -1.0 * dy;
	//cv::normalize(dx, dx, 0.0, 1.0, cv::NORM_MINMAX);
	//cv::normalize(dy, dy, 0.0, 1.0, cv::NORM_MINMAX);
	//cv::Mat dx_r, dy_r;
	//cv::resize(dy, dy_r, cv::Size(500, 500));
	//cv::resize(dx, dx_r, cv::Size(500, 500));
	//dx_r = dx_r.t();
	//cv::imshow("dy", dy_r);
	//cv::imshow("dx", dx_r);
	//cv::waitKey(0);

	cv::Mat gtImg = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1 gt.png", 0);
	//cv::Mat gtImg = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Vaz 1/test.png", 0);
	cv::Mat image = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1.png", 0);
	cv::Mat vazImg = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1 vaz.png", 0);

	std::vector<dune::DuneSegment> segments;
	segments.push_back(dune::DuneSegment());

	std::vector<dune::DuneSegmentData> data;
	for (int i = 0; i < vazImg.rows; ++i)
	{
		for (int j = 0; j < vazImg.cols; ++j)
		{
			if (vazImg.at<uchar>(i,j) == 0)
				continue;

			data.push_back(dune::DuneSegmentData(cv::Point(j, i), 0.0));
		}
	}
	segments[0].SetSegmentData(data);

	std::ofstream resultsDatFile(DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1_vaz_results_.dat");
	for (int i = 2; i <= 50; i += 2)
	{
		dune::BaseDuneDetectorBenchmark benchmark;
		benchmark.BenchmarkParams.MinError = i;
		dune::BenchmarkResults benchmarkResults = benchmark.GetResults(image, gtImg, segments, DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1_vaz_results_.png");
		resultsDatFile << i << '\t' << benchmarkResults.GetPrecision() << '\t' << benchmarkResults.GetRecall() << std::endl;
	}
	resultsDatFile.close();

	dune::BaseDuneDetectorBenchmark benchmark;
	benchmark.BenchmarkParams.MinError = 10.0;
	dune::BenchmarkResults benchmarkResults = benchmark.GetResults(image, gtImg, segments, DUNE_ML_DATASET_BASE_PATH + "Vaz 1/Area 1_vaz_results_.png");

	std::cout << "Benchmark results: " << std::endl
		<< "\tTP: " << benchmarkResults.TP << std::endl
		<< "\tFP: " << benchmarkResults.FP << std::endl
		/*<< "\tTN: " << benchmarkResults.TN << std::endl
		<< "\tFN: " << benchmarkResults.FN << std::endl*/
		<< "\tPrecision: " << benchmarkResults.GetPrecision() << std::endl
		<< "\tRecall: " << benchmarkResults.GetRecall() << std::endl
		<< "\tAngular Error: " << benchmarkResults.AngularError << std::endl
		<< "\tSpacing Error: " << benchmarkResults.SpacingError << std::endl
		<< "\tNormalized Spacing Error: " << benchmarkResults.NormSpacingError << std::endl;







	//cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg", 0);
	//cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg", 0);
	//cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg", 0);

	/*cv::medianBlur(testImage, testImage, 7);
	dune::EdgeDirectionDuneDetector detector(
		new dune::EdgeDirectionImageProcessor(new dune::EdgeDirectionProcParams(9, 16, true, -0.2979)),
		new dune::EdgeDirectionDetectorParameters());

	std::vector<dune::DuneSegment> segments = detector.Extract(testImage);*/
	/*ImageVectorDraw ivd;
	ivd.Begin(testImage);

	std::vector<VectorPlot> plots = ivd.GetVectors();
	cv::Mat display = ivd.GetDisplay();

	for (int i = 0; i < plots.size(); ++i)
	{
		std::ofstream plotFile(DUNE_ML_DATASET_BASE_PATH + "Skeleton plot.txt");
		for (int j = 0; j < plots[i].angle.size(); ++j)
		{
			plotFile << plots[i].intensity[j] << '\t'
				<< plots[i].angle[j] << '\t'
				<< plots[i].magnitude[j] << std::endl;
 		}
		cv::imwrite(DUNE_ML_DATASET_BASE_PATH + "Skeleton display.jpg", display);
		plotFile.close();
	}*/


	//
	//cv::GaussianBlur(testImage, testImage, cv::Size(7, 7), 1.5, 1.5);
	//
	//cv::Mat processed;
	//dune::imgproc::LocalScaleIntegralIlluminationNormalization(testImage, processed);
	//cv::equalizeHist(processed, processed);
	//cv::imshow("processed", processed);
	//cv::waitKey(0);

	system("PAUSE");
	return 0;
}