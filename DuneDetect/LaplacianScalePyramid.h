#pragma once

#include "OpenCVHeaders.h"

class LaplacianScalePyramid
{
public:
	LaplacianScalePyramid();
	~LaplacianScalePyramid();

	void Process(const cv::Mat &img);

	double GetScaleAt(int x, int y);

	cv::Mat GetScaleMap() { return _scaleMap; }

private:
	std::vector<cv::Mat> _pyramid;
	std::vector<double> _sigmaValues;
	std::vector<unsigned int> _kernelSizes;
	int _numScales;
	cv::Mat _scaleMap;

	void init();
	void computeScaleMap();
	double solveScale(cv::Point2d pl, cv::Point2d p0, cv::Point2d pr);
};
