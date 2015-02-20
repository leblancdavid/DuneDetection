#ifndef _DUNE_SEGMENT_DETECTOR_H_
#define _DUNE_SEGMENT_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "AdaptiveImageProcessor.h"

namespace dune
{

class DuneSegmentDetectorParameters
{
public:
	DuneSegmentDetectorParameters()
	{
		MinSegmentLength = 100;
		GaussianKernelSize = 51;
		GaussianKernelSigma = (double)GaussianKernelSigma/6.4251;
		CurvatureKernelSize = 21;
		CurvatureKernelSigma = (double)CurvatureKernelSize/6.4251;
	}
	DuneSegmentDetectorParameters(const DuneSegmentDetectorParameters &cpy)
	{
		MinSegmentLength = cpy.MinSegmentLength;
		GaussianKernelSize = cpy.GaussianKernelSize;
		GaussianKernelSigma = cpy.GaussianKernelSigma;
		CurvatureKernelSize = cpy.CurvatureKernelSize;
		CurvatureKernelSigma = cpy.CurvatureKernelSize;
	}
	DuneSegmentDetectorParameters(int pMinContourLength, 
		int pKernelSize, 
		double pKernelSigma,
		int pCurveSize,
		double pCurveSigma)
	{
		MinSegmentLength = pMinContourLength;
		GaussianKernelSize = pKernelSize;
		GaussianKernelSigma = pKernelSigma;
		CurvatureKernelSize = pCurveSize;
		CurvatureKernelSigma = pCurveSigma;
	}

	int MinSegmentLength;
	int GaussianKernelSize;
	double GaussianKernelSigma;
	int CurvatureKernelSize;
	double CurvatureKernelSigma;
};

class DuneSegmentDetector : public BaseDuneDetector
{
public:
	DuneSegmentDetector();
	DuneSegmentDetector(BaseImageProcessor* imgproc, const DuneSegmentDetectorParameters &params);
	~DuneSegmentDetector();

	std::vector<DuneSegment> Extract(const cv::Mat &img);

private:

	std::vector<double> CurvatureKernel;
	std::vector<double> GaussianKernel;

	void InitGaussianKernel(int size, double sigma);
	void InitCurvatureKernel(int size, double sigma);

	DuneSegmentDetectorParameters Parameters;
	std::vector<std::vector<cv::Point>> GetContours(const cv::Mat &img);
	std::vector<std::vector<cv::Point>> SplitContourSegments(const std::vector<cv::Point> &contour);

	std::vector<double> CalcContourCurvature(const std::vector<cv::Point> &contour);
	void GaussianSmoothSegment(std::vector<cv::Point> &contour);
	std::vector<double> CalcContourDerivative(const cv::Mat &derivativeImg, const std::vector<cv::Point> &segment);
	double CalcSegmentAverageDerivative(const cv::Mat &derivativeImg, const std::vector<cv::Point> &segment);
};

}
#endif