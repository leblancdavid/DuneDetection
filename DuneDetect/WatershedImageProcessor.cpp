#include "WatershedImageProcessor.h"
#include "DuneMathTools.h"

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

WatershedImageProcessor::WatershedImageProcessor(const WatershedProcessorParameters &params)
{

}

void WatershedImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	double k = Parameters.K;
	//PreProcess the image
	cv::Mat filtered;
	cv::medianBlur(inputImg, filtered, k);
	cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 4.7, (double)k / 4.7);
	//cv::equalizeHist(filtered, filtered);

	//Get the derivatives
	cv::Sobel(filtered, xDeriv, CV_64F, 1, 0, Parameters.K);
	cv::Sobel(filtered, yDeriv, CV_64F, 0, 1, Parameters.K);

	//Perform the watershed
	WatershedSegmentation(filtered, outputImg);
}

void WatershedImageProcessor::WatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	std::vector<double> orientations = FindDominantOrientations();

	cv::Mat dMag(inputImg.rows, inputImg.cols, CV_64F);
	cv::Mat dDir(inputImg.rows, inputImg.cols, CV_64F);
	double maxMag = 0;
	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			double xVal = xDeriv.at<double>(y, x);
			double yVal = yDeriv.at<double>(y, x);
			double m = std::sqrt(xVal*xVal + yVal*yVal);
			dMag.at<double>(y, x) = m;
			dDir.at<double>(y, x) = std::atan2(yVal, xVal);
		}
	}

	cv::normalize(dMag, dMag, 0, 255.0, cv::NORM_MINMAX);
	
	cv::Mat magImg;
	dMag.convertTo(magImg, CV_8UC1);

	//First find the mean value
	cv::Scalar mean, stdev;
	cv::meanStdDev(magImg, mean, stdev);

	cv::Mat sun, shade;

	uchar sunVal = 255;
	uchar shadeVal = 32;
	uchar increments = (uchar)std::floor((double)(sunVal - shadeVal) / ((double)orientations.size() + 1.0));
	cv::threshold(magImg, sun, mean[0] + Parameters.HighQ*stdev[0], sunVal, CV_THRESH_BINARY);
	cv::threshold(magImg, shade, mean[0] + Parameters.LowQ*stdev[0], shadeVal, CV_THRESH_BINARY_INV);

	cv::erode(sun, sun, cv::Mat(), cv::Point(-1, -1), 1);
	cv::erode(shade, shade, cv::Mat(), cv::Point(-1, -1), 1);
	cv::Mat seedImg = sun + shade;
	cv::Mat watershedImg(inputImg.rows, inputImg.cols, CV_8UC3);

	for (int x = 0; x < seedImg.cols; ++x)
	{
		for (int y = 0; y < seedImg.rows; ++y)
		{
			if (seedImg.at<uchar>(y, x) == 255)
			{
				double xVal = xDeriv.at<double>(y, x);
				double yVal = yDeriv.at<double>(y, x);
				int minIndex;
				double minAngle = dune::math::TWO_PI;
				double angle = std::atan2(yVal, xVal);
				for (int i = 0; i < orientations.size(); ++i)
				{
					double a = dune::math::minAngleDistance(angle, orientations[i]);
					if (a < minAngle)
					{
						minIndex = i;
						minAngle = a;
					}
				}

				seedImg.at<uchar>(y, x) = increments*(minIndex+1) + shadeVal;
			}

			double xVal = std::fabs(dMag.at<double>(y, x)*std::cos(dDir.at<double>(y, x)));
			double yVal = std::fabs(dMag.at<double>(y, x)*std::sin(dDir.at<double>(y, x)));
			watershedImg.at<cv::Vec3b>(y, x)[0] = (uchar)xVal;
			watershedImg.at<cv::Vec3b>(y, x)[1] = (uchar)yVal;
			watershedImg.at<cv::Vec3b>(y, x)[2] = 0;
		}
	}

	cv::imshow("Seed Image", seedImg);
	cv::imshow("Watershed Image", watershedImg);
	cv::waitKey(0);

	cv::Mat markers;
	seedImg.convertTo(markers, CV_32S);

	cv::watershed(watershedImg, markers);
	markers.convertTo(seedImg, CV_8UC1);
	cv::imshow("Watershed Results", seedImg);
	cv::waitKey(0);


}

std::vector<double> WatershedImageProcessor::FindDominantOrientations()
{
	cv::Scalar mean_dx, mean_dy, stddev;

	cv::meanStdDev(xDeriv, mean_dx, stddev);
	cv::meanStdDev(yDeriv, mean_dy, stddev);

	double avgOrientation = std::atan2(mean_dy[0], mean_dx[0]);

	if (avgOrientation < 0)
		avgOrientation += 2.0*3.1416;

	double increments = 2.0*3.1416 / (double)Parameters.HistogramBins;
	std::vector<double> HoG(Parameters.HistogramBins);
	std::vector<cv::Point2d> HoG2D(Parameters.HistogramBins);
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		HoG2D[i].x = 0.0;
		HoG2D[i].y = 0.0;
	}

	for (int x = 0; x < xDeriv.cols; ++x)
	{
		for (int y = 0; y < xDeriv.rows; ++y)
		{
			double dx = xDeriv.at<double>(y, x) - mean_dx[0];
			double dy = yDeriv.at<double>(y, x) - mean_dy[0];
			double mag = std::sqrt(dx*dx + dy*dy);

			double orientation = std::atan2(dy, dx) - avgOrientation;
			while (orientation < 0)
				orientation += 2.0*3.1416;

			int bin = std::ceil((orientation / increments) - 0.5);
			if (bin >= Parameters.HistogramBins)
				bin = 0;

			HoG2D[bin].x += dx / 1000.0;
			HoG2D[bin].y += dy / 1000.0;
		}
	}


	int index = 0;
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		double dx = HoG2D[i].x / 1000.0;
		double dy = HoG2D[i].y / 1000.0;
		HoG[i] = std::sqrt(dx*dx + dy*dy);
	}
	
	std::vector<double> orientations;

	for (int i = 0; i < HoG.size(); ++i)
	{
		int p = i - 1;
		int n = i + 1;
		if (p < 0)
			p = HoG.size() - 1;
		if (n >= HoG.size())
			n = 0;

		if (HoG[i] > HoG[p] && HoG[i] > HoG[n])
		{
			double dx = HoG2D[i].x / 1000.0;
			double dy = HoG2D[i].y / 1000.0;
			orientations.push_back(std::atan2(dy, dx));
		}
	}


	return orientations;
}

}