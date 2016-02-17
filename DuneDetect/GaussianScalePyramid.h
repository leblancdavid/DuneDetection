#pragma once


#include "OpenCVHeaders.h"

class GaussianScalePyramid
{
public:
	GaussianScalePyramid();
	~GaussianScalePyramid();

	void Process(const cv::Mat &img);

	double GetScaleAt(int x, int y);

	cv::Mat GetScaleMap() { return _scaleMap; }

	cv::Mat GetScaleImage() { return _scaleImage; }

private:
	std::vector<cv::Mat> _pyramid;
	std::vector<double> _sigmaValues;
	std::vector<unsigned int> _kernelSizes;
	int _numScales;
	cv::Mat _scaleMap;
	cv::Mat _scaleImage;

	void init();
	void computeScaleMap();
	void computeScaleImage();
	double subPixelCalc(cv::Point2d pl, cv::Point2d p0, cv::Point2d pr);
	double GetLocalPyrVal(int index, int x, int y, int k);
	double getPyrValAt(double sigma);
};