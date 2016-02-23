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
	////cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg", 0);
	////cv::Mat sobel, testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg", 0);

	//LaplacianScalePyramid lsp;
	//GaussianScalePyramid gsp;
	//gsp.Process(testImage);
	//cv::Mat scaleMap = gsp.GetScaleMap();
	//cv::normalize(scaleMap, scaleMap, 255.0, 0.0, cv::NORM_MINMAX);
	//cv::Mat scale;
	//scaleMap.convertTo(scale, CV_8U);
	//cv::imshow("scale", scale);
	//cv::Mat scaleImage = gsp.GetScaleImage();
	//cv::imshow("scaleImage", scaleImage);

	//cv::imwrite("Kal_scale.jpg", scale);
	//cv::imwrite("Kal_scaleImg.jpg", scaleImage);

	//cv::waitKey(0);


	//dune::EdgeDirectionImageProcessor ip;
	//dune::SuperDuperImageProcessor ip;

	//cv::Mat processed;
	//ip.Process(testImage, processed);
	//cv::imshow("processed", processed);
	//cv::waitKey(0);

	////cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "WDC 1/WCD-1 test.jpg", 0);
	////cv::Mat testImage = cv::imread(DUNE_ML_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3 test.jpg", 0);

	//cv::medianBlur(testImage, testImage, 5);

	////dune::imgproc::GaborFilterImageEnhancement imgEnhancer;
	////dune::imgproc::DFTImageEnhancement imgEnhancer;
	//cv::Mat outputImage;
	////imgEnhancer.Process(testImage, outputImage);
	//dune::imgproc::IntegralIlluminationNormalization(testImage, outputImage, 10);
	//cv::imshow("testImage", testImage);
	//cv::imshow("outputImage", outputImage);
	//cv::waitKey(0);

	//dune::imgproc::DiscreteFourierTransform dft;
	//cv::Mat spectrumImage;
	//dft.Forward(testImage, spectrumImage);
	////cv::imshow("Spectrum", spectrumImage);
	////cv::waitKey(0);

	//cv::RotatedRect ellipse;
	//dft.GetEllipseFit(ellipse, 1.0);

	//dune::imgproc::GaborFilter gf;
	//cv::Mat filter = gf.GetFrequencySpaceFilter(spectrumImage.rows,
	//	spectrumImage.cols,
	//	ellipse.size.height,
	//	ellipse.size.width,
	//	0.64);
	////invert the filter
	////filter = cv::Scalar(1.0) - filter;

	//dft.MaskFilter(filter, spectrumImage);
	//cv::imshow("GF", filter);
	//cv::imshow("Spectrum", spectrumImage);
	//cv::waitKey(0);

	//system("PAUSE");
	return 0;
}