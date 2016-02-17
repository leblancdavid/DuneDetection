#include "WatershedImageProcessor.h"
#include "DuneMath.h"
#include "GenericImageProcessing.h"

namespace dune
{

WatershedImageProcessor::WatershedImageProcessor()
{

}

WatershedImageProcessor::~WatershedImageProcessor()
{

}

WatershedImageProcessor::WatershedImageProcessor(const WatershedImageProcessor &cpy)
{

}

WatershedImageProcessor::WatershedImageProcessor(WatershedProcessorParameters *params)
{
	Parameters = params;
}

void WatershedImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	double k = Parameters->K;
	//PreProcess the image
	cv::Mat filtered;
	cv::medianBlur(inputImg, filtered, k);
	cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 4.7, (double)k / 4.7);
	//cv::equalizeHist(filtered, filtered);

	//Get the derivatives
	ComputeGradient(filtered, k);

	//This normalizes the illumination of the image, making segmentation slightly easier.
	cv::Mat normalizedIllumination;
	imgproc::IntegralIlluminationNormalization(filtered, normalizedIllumination, Parameters->Radius);
	//ComputeGradient(normalizedIllumination, k);
	//Perform the watershed
	WatershedSegmentationIntensityBased(normalizedIllumination, outputImg);

}

void WatershedImageProcessor::WatershedSegmentationIntensityBased(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	cv::Scalar meanIntensity, stdDevIntensity;
	cv::meanStdDev(inputImg, meanIntensity, stdDevIntensity);

	//First find the mean value
	cv::Mat sun, shade;
	double nonseedThreshold = meanIntensity[0] + Parameters->LowQ * stdDevIntensity[0];
	double seedThreshold = meanIntensity[0] + Parameters->HighQ * stdDevIntensity[0];


	cv::adaptiveThreshold(inputImg, sun, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, Parameters->Radius*2.0 + 1, -0.2 * Parameters->Radius);
	cv::adaptiveThreshold(inputImg, shade, 128, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, Parameters->Radius*2.0 + 1, -0.2 * Parameters->Radius);

	//cv::threshold(inputImg, sun, seedThreshold, 255, CV_THRESH_BINARY);
	//cv::threshold(inputImg, shade, nonseedThreshold, 128, CV_THRESH_BINARY_INV);

	cv::erode(shade, shade, cv::Mat(), cv::Point(-1, -1), 5);
	cv::erode(sun, sun, cv::Mat(), cv::Point(-1, -1), 3);
	cv::Mat seedImg = sun + shade;

	cv::imshow("seedImg", seedImg);
	cv::waitKey(0);

	//cv::imwrite("shadedlabel.jpg", shade);
	//cv::imwrite("sunnylabel.jpg", sun);

	cv::Mat markers;
	seedImg.convertTo(markers, CV_32S);

	cv::Mat watershedImg(inputImg.rows, inputImg.cols, CV_8UC3);
	cv::cvtColor(inputImg, watershedImg, CV_GRAY2BGR);
	
	/*cv::Mat magnitudeImg = BaseData.toImage(GRADIENT_MAT_MAGNITUDE_INDEX);
	for (int x = 0; x < watershedImg.cols; ++x)
	{
		for (int y = 0; y < watershedImg.rows; ++y)
		{
			watershedImg.at<cv::Vec3b>(y, x)[1] = 255 - magnitudeImg.at<uchar>(y, x);
		}
	}*/
	
	cv::watershed(watershedImg, markers);
	markers.convertTo(outputImg, CV_8UC1);

	//cv::imwrite("watershedResultingMarkers.jpg", outputImg);
	//cv::imshow("markers", outputImg);
	cv::threshold(outputImg, outputImg, 200, 255, CV_THRESH_BINARY);
	//cv::imshow("watershedImg", watershedImg);
	//cv::imshow("outputImg", outputImg);
	//cv::waitKey(0);

	//cv::erode(outputImg, outputImg, cv::Mat(), cv::Point(-1, -1), 1);
	//cv::medianBlur(outputImg, outputImg, Parameters.K);
	/*
	cv::imshow("Seed Image", seedImg);
	//cv::waitKey(0);
	markers.convertTo(seedImg, CV_8UC1);
	cv::imshow("Watershed Results", seedImg);

	cv::Mat overlapImg;
	cv::cvtColor(inputImg, overlapImg, CV_GRAY2BGR);
	for (int x = 0; x < overlapImg.cols; ++x)
	{
		for (int y = 0; y < overlapImg.rows; ++y)
		{
			overlapImg.at<cv::Vec3b>(y, x)[0] = seedImg.at<uchar>(y, x);
		}
	}
	cv::imshow("Overlap", overlapImg);
	cv::waitKey(0);*/

}

void WatershedImageProcessor::CannyBasedWatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	double dominantOrientation = ComputeDominantOrientation(DominantOrientationMethod::HOG, Parameters->HistogramBins);

	cv::Mat canny;
	double HighT = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
	cv::Canny(inputImg, canny, HighT, HighT / 2.0, Parameters->K, true);

	cv::Mat dMag(canny.rows, canny.cols, CV_64F);
	cv::Mat dDir(canny.rows, canny.cols, CV_64F);
	for (int x = 0; x < canny.cols; ++x)
	{
		for (int y = 0; y < canny.rows; ++y)
		{
			if (canny.at<uchar>(y, x))
			{
				dMag.at<double>(y, x) = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_MAGNITUDE_INDEX];
				dDir.at<double>(y, x) = std::fabs(math::PI - std::fabs(BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DIRECTION_INDEX] - dominantOrientation));
			}
			else
			{
				dMag.at<double>(y, x) = 0.0;
				dDir.at<double>(y, x) = 0.0;
			}
			
		}
	}

	cv::normalize(dMag, dMag, 0, 255.0, cv::NORM_MINMAX);
	cv::normalize(dDir, dDir, 0, 255.0, cv::NORM_MINMAX);

	cv::Mat magImg, dirImg, seedImg;
	dMag.convertTo(magImg, CV_8UC1);
	dDir.convertTo(dirImg, CV_8UC1);

	//First find the mean value
	cv::Mat sun, shade;
	double nonseedThreshold = Parameters->AngleTolerance / math::PI * 255.0;
	double seedThreshold = (math::PI - Parameters->AngleTolerance) / math::PI * 255.0;

	cv::threshold(dirImg, sun, seedThreshold, 255, CV_THRESH_BINARY);
	cv::threshold(dirImg, shade, nonseedThreshold, 128, CV_THRESH_BINARY_INV);

	cv::erode(shade, shade, cv::Mat(), cv::Point(-1, -1), 5);
	//cv::dilate(sun, sun, cv::Mat(), cv::Point(-1, -1), 1);
	seedImg = sun + shade;

	cv::Mat watershedImg(inputImg.rows, inputImg.cols, CV_8UC3);

	for (int x = 0; x < watershedImg.cols; ++x)
	{
		for (int y = 0; y < watershedImg.rows; ++y)
		{
			watershedImg.at<cv::Vec3b>(y, x)[0] = magImg.at<uchar>(y, x);
			watershedImg.at<cv::Vec3b>(y, x)[1] = dirImg.at<uchar>(y, x);
			watershedImg.at<cv::Vec3b>(y, x)[2] = canny.at<uchar>(y,x);
		}
	}

	cv::imshow("DirImg", dirImg);
	cv::imshow("Seed Image", seedImg);
	cv::imshow("Watershed Image", watershedImg);
	cv::waitKey(0);

	cv::Mat markers;
	seedImg.convertTo(markers, CV_32S);

	cv::watershed(watershedImg, markers);
	markers.convertTo(seedImg, CV_8UC1);
	cv::imshow("Watershed Results", seedImg);

	cv::Mat overlapImg;
	cv::cvtColor(inputImg, overlapImg, CV_GRAY2BGR);
	for (int x = 0; x < overlapImg.cols; ++x)
	{
		for (int y = 0; y < overlapImg.rows; ++y)
		{
			overlapImg.at<cv::Vec3b>(y, x)[0] = seedImg.at<uchar>(y, x);
		}
	}
	cv::imshow("Overlap", overlapImg);
	cv::waitKey(0);

}

void WatershedImageProcessor::WatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	//std::vector<double> orientations = FindDominantOrientations();
	double dominantOrientation = ComputeDominantOrientation(DominantOrientationMethod::HOG, Parameters->HistogramBins);

	cv::Mat dMag(inputImg.rows, inputImg.cols, CV_64F);
	cv::Mat dDir(inputImg.rows, inputImg.cols, CV_64F);
	double maxMag = 0;
	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			dMag.at<double>(y, x) = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_MAGNITUDE_INDEX];
			dDir.at<double>(y, x) = std::fabs(math::PI - std::fabs(BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DIRECTION_INDEX] - dominantOrientation));
		}
	}

	cv::normalize(dMag, dMag, 0, 1.0, cv::NORM_MINMAX);
	cv::normalize(dDir, dDir, 0, 1.0, cv::NORM_MINMAX);
	//cv::divide(dDir, dMag, dDir, CV_64F);

	cv::pow(dMag, 0.5, dMag);
	cv::pow(dDir, 0.5, dDir);
	dDir = dDir + dMag;
	cv::normalize(dDir, dDir, 0, 255.0, cv::NORM_MINMAX);
	cv::normalize(dDir, dDir, 0, 255.0, cv::NORM_MINMAX);

	cv::Mat magImg, dirImg, seedImg;
	dMag.convertTo(magImg, CV_8UC1);
	dDir.convertTo(dirImg, CV_8UC1);
	
	

	//First find the mean value
	cv::Mat sun, shade;
	double nonseedThreshold = Parameters->AngleTolerance / math::PI * 255.0;
	double seedThreshold = (math::PI - Parameters->AngleTolerance) / math::PI * 255.0;
	cv::threshold(dirImg, sun, seedThreshold, 255, CV_THRESH_BINARY);
	cv::threshold(dirImg, shade, nonseedThreshold, 128, CV_THRESH_BINARY_INV);

	cv::erode(shade, shade, cv::Mat(), cv::Point(-1, -1), 1);
	cv::erode(sun, sun, cv::Mat(), cv::Point(-1, -1), 4);
	seedImg = sun + shade;

	cv::Mat watershedImg(inputImg.rows, inputImg.cols, CV_8UC3);

	for (int x = 0; x < watershedImg.cols; ++x)
	{
		for (int y = 0; y < watershedImg.rows; ++y)
		{
			watershedImg.at<cv::Vec3b>(y, x)[0] = magImg.at<uchar>(y,x);
			watershedImg.at<cv::Vec3b>(y, x)[1] = dirImg.at<uchar>(y, x);
			watershedImg.at<cv::Vec3b>(y, x)[2] = 0;
		}
	}

	cv::imshow("DirImg", dirImg);
	cv::imshow("Seed Image", seedImg);
	cv::imshow("Watershed Image", watershedImg);
	cv::waitKey(0);

	cv::Mat markers;
	seedImg.convertTo(markers, CV_32S);

	cv::watershed(watershedImg, markers);
	markers.convertTo(seedImg, CV_8UC1);
	cv::imshow("Watershed Results", seedImg);

	cv::Mat overlapImg;
	cv::cvtColor(inputImg, overlapImg, CV_GRAY2BGR);
	for (int x = 0; x < overlapImg.cols; ++x)
	{
		for (int y = 0; y < overlapImg.rows; ++y)
		{
			overlapImg.at<cv::Vec3b>(y, x)[0] = seedImg.at<uchar>(y, x);
		}
	}
	cv::imshow("Overlap", overlapImg);
	cv::waitKey(0);


}

}