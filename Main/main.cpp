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


int main()
{

	//RunDuneLibTest();
	RunMLTest();

	//cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Kalahari 3/Kalahari 3 image.jpg", 0);
	//cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg", 0);
	//cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg", 0);
	//
	//cv::medianBlur(testImage, testImage, 7);
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