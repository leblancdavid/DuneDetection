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
		MinContourLength = 50;
		KernelSize = 15;
		KernelSigma = (double)KernelSize/6.4251;
	}
	DuneSegmentDetectorParameters(const DuneSegmentDetectorParameters &cpy)
	{
		MinContourLength = cpy.MinContourLength;
		KernelSize = cpy.KernelSize;
		KernelSigma = cpy.KernelSigma;
	}
	DuneSegmentDetectorParameters(int pMinContourLength, int pKernelSize, double pKernelSigma)
	{
		MinContourLength = pMinContourLength;
		KernelSize = pKernelSize;
		KernelSigma = pKernelSigma;
	}

	int MinContourLength;
	int KernelSize;
	double KernelSigma;
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
};

}
#endif