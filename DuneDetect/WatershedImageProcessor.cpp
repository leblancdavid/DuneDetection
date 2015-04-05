#include "WatershedImageProcessor.h"
#include "DuneMath.h"

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
	ComputeGradient(filtered, k);
	
	cv::Mat normalizedIllumination;
	NormalizeIllumination(filtered, normalizedIllumination);


	//Perform the watershed
	WatershedSegmentationIntensityBased(normalizedIllumination, outputImg);
	//WatershedSegmentation(normalizedIllumination, outputImg);
	//CannyBasedWatershedSegmentation(filtered, outputImg);
}

void WatershedImageProcessor::WatershedSegmentationIntensityBased(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	cv::Scalar meanIntensity, stdDevIntensity;
	cv::meanStdDev(inputImg, meanIntensity, stdDevIntensity);

	//First find the mean value
	cv::Mat sun, shade;
	double nonseedThreshold = meanIntensity[0] + Parameters.LowQ * stdDevIntensity[0];
	double seedThreshold = meanIntensity[0] + Parameters.HighQ * stdDevIntensity[0];

	cv::threshold(inputImg, sun, seedThreshold, 255, CV_THRESH_BINARY);
	cv::threshold(inputImg, shade, nonseedThreshold, 128, CV_THRESH_BINARY_INV);

	cv::erode(shade, shade, cv::Mat(), cv::Point(-1, -1), 3);
	cv::erode(sun, sun, cv::Mat(), cv::Point(-1, -1), 1);
	cv::Mat seedImg = sun + shade;


	cv::imshow("Seed Image", seedImg);
	//cv::waitKey(0);

	cv::Mat markers;
	seedImg.convertTo(markers, CV_32S);

	cv::Mat watershedImg(inputImg.rows, inputImg.cols, CV_8UC3);
	cv::cvtColor(inputImg, watershedImg, CV_GRAY2BGR);

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

void WatershedImageProcessor::CannyBasedWatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	double dominantOrientation = FindDominantOrientation();

	cv::Mat canny;
	double HighT = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
	cv::Canny(inputImg, canny, HighT, HighT / 2.0, Parameters.K, true);

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
	double nonseedThreshold = Parameters.AngleTolerance / math::PI * 255.0;
	double seedThreshold = (math::PI - Parameters.AngleTolerance) / math::PI * 255.0;

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
	double dominantOrientation = FindDominantOrientation();

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
	double nonseedThreshold = Parameters.AngleTolerance / math::PI * 255.0;
	double seedThreshold = (math::PI - Parameters.AngleTolerance) / math::PI * 255.0;
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

double WatershedImageProcessor::FindDominantOrientation()
{
	double avgOrientation = BaseData.Mean[GRADIENT_MAT_DIRECTION_INDEX];

	double increments = 2.0*3.1416 / (double)Parameters.HistogramBins;
	std::vector<double> HoG(Parameters.HistogramBins);
	std::vector<cv::Point2d> HoG2D(Parameters.HistogramBins);
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		HoG2D[i].x = 0.0;
		HoG2D[i].y = 0.0;
	}

	for (int x = 0; x < BaseData.Gradient.cols; ++x)
	{
		for (int y = 0; y < BaseData.Gradient.rows; ++y)
		{
			double dx = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DX_INDEX] - BaseData.Mean[GRADIENT_MAT_DX_INDEX];
			double dy = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DY_INDEX] - BaseData.Mean[GRADIENT_MAT_DY_INDEX];

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


	double dominantDir;
	double dominantMag = 0;
	for (int i = 0; i < HoG.size(); ++i)
	{
		int p = i - 1;
		int n = i + 1;
		if (p < 0)
			p = HoG.size() - 1;
		if (n >= HoG.size())
			n = 0;

		if (HoG[i] > HoG[p] && HoG[i] > HoG[n] && HoG[i] > dominantMag)
		{
			dominantMag = HoG[i];

			double dx = HoG2D[i].x;
			double dy = HoG2D[i].y;

			dominantDir = std::atan2(dy, dx);
		}
	}


	return dominantDir;
}

std::vector<double> WatershedImageProcessor::FindDominantOrientations()
{
	double avgOrientation = BaseData.Mean[GRADIENT_MAT_DIRECTION_INDEX];

	double increments = 2.0*3.1416 / (double)Parameters.HistogramBins;
	std::vector<double> HoG(Parameters.HistogramBins);
	std::vector<cv::Point2d> HoG2D(Parameters.HistogramBins);
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		HoG2D[i].x = 0.0;
		HoG2D[i].y = 0.0;
	}

	for (int x = 0; x < BaseData.Gradient.cols; ++x)
	{
		for (int y = 0; y < BaseData.Gradient.rows; ++y)
		{
			double dx = BaseData.Gradient.at<double>(y, x) - BaseData.Mean[GRADIENT_MAT_DX_INDEX];
			double dy = BaseData.Gradient.at<double>(y, x) - BaseData.Mean[GRADIENT_MAT_DY_INDEX];

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
	
	
	double dominantDir;
	double dominantMag = 0;
	for (int i = 0; i < HoG.size(); ++i)
	{
		int p = i - 1;
		int n = i + 1;
		if (p < 0)
			p = HoG.size() - 1;
		if (n >= HoG.size())
			n = 0;

		if (HoG[i] > HoG[p] && HoG[i] > HoG[n] && HoG[i] > dominantMag)
		{
			dominantMag = HoG[i];

			double dx = HoG2D[i].x;
			double dy = HoG2D[i].y;

			dominantDir = std::atan2(dy, dx);
		}
	}

	std::vector<double> orientations;
	orientations.push_back(dominantDir);
	return orientations;
}

void WatershedImageProcessor::NormalizeIllumination(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	cv::Mat integralImg;
	cv::integral(inputImg, integralImg, CV_64F);

	cv::Scalar meanIntensity, stdDevIntensity;
	cv::meanStdDev(inputImg, meanIntensity, stdDevIntensity);

	cv::Point A, B, C, D;
	outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC1);
	
	int radius = 50;

	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			int left = x - Parameters.Radius;
			int right = x + Parameters.Radius;
			int top = y - Parameters.Radius;
			int bottom = y + Parameters.Radius;

			if (left < 0)
				left = 0;
			if (right >= integralImg.cols)
				right = integralImg.cols - 1;
			if (top < 0)
				top = 0;
			if (bottom >= integralImg.rows)
				bottom = integralImg.rows - 1;

			int width = right - left;
			int height = bottom - top;
			double area = width * height;

			A.x = left, A.y = top;
			B.x = right, B.y = top;
			C.x = left, C.y = bottom;
			D.x = right, D.y = bottom;

			double sum = integralImg.at<double>(D) - integralImg.at<double>(C) - integralImg.at<double>(B) + integralImg.at<double>(A);
			
			double r = meanIntensity[0] / (sum / area);
			double outputVal = std::ceil((double)inputImg.at<uchar>(y, x) * r - 0.5);
			if (outputVal > 255.0)
				outputVal = 255.0;
			else if (outputVal < 0)
				outputVal = 0;

			//double r = meanIntensity[0] - (sum / area);
			//double outputVal = std::ceil((double)inputImg.at<uchar>(y, x) + r - 0.5);
			//if (outputVal > 255.0)
			//	outputVal = 255.0;
			//else if (outputVal < 0)
			//	outputVal = 0;

			outputImg.at<uchar>(y, x) = (uchar)outputVal;
		}
	}

	//cv::imshow("Input Image", inputImg);
	//cv::imshow("Normalized Illumination", outputImg);

	//cv::Mat diff = (outputImg - inputImg);
	//cv::normalize(diff, diff, 0, 255, cv::NORM_MINMAX);

	//cv::Mat equalized;
	//cv::equalizeHist(outputImg, equalized);
	//cv::Mat threshold1, threshold2;
	//cv::threshold(outputImg, threshold1, meanIntensity[0] + Parameters.HighQ * stdDevIntensity[0], 255.0, CV_THRESH_BINARY);
	//cv::imshow("Diff Image", diff);
	//cv::imshow("Equalized", equalized);
	//cv::imshow("Thres1", threshold1);
	//cv::threshold(inputImg, threshold2, meanIntensity[0] + Parameters.HighQ * stdDevIntensity[0], 255.0, CV_THRESH_BINARY);
	//cv::imshow("Thres2", threshold2);
	//cv::waitKey(0);

	//cv::equalizeHist(outputImg, outputImg);
}

}