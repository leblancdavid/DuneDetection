#ifndef _WATERSHED_DUNE_DETECTOR_H_
#define _WATERSHED_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "WatershedImageProcessor.h"

namespace dune
{

	class WatershedDuneDetectorParameters
	{
	public:
		WatershedDuneDetectorParameters()
		{
			K = 7;
			R = 0.1;
			MinSegmentLength = 10;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
		}
		WatershedDuneDetectorParameters(const WatershedDuneDetectorParameters &cpy)
		{
			K = cpy.K;
			R = cpy.R;
			MinSegmentLength = cpy.MinSegmentLength;
			HistogramBins = cpy.HistogramBins;
			AngleTolerance = cpy.AngleTolerance;
		}
		WatershedDuneDetectorParameters(int pK, double pR, int pMinSegmentLength, int pHistBins, double pAngleTolerance)
		{
			K = pK;
			R = pR;
			MinSegmentLength = pMinSegmentLength;
			HistogramBins = pHistBins;
			AngleTolerance = pAngleTolerance;
		}

		int K;
		double R;
		int MinSegmentLength;
		int HistogramBins;
		double AngleTolerance;

	};

	class WatershedDuneDetector : public BaseDuneDetector
	{
	public:
		WatershedDuneDetector();
		WatershedDuneDetector(WatershedImageProcessor* imgproc, const WatershedDuneDetectorParameters &params);
		~WatershedDuneDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

	private:

		WatershedDuneDetectorParameters Parameters;

		std::vector<std::vector<cv::Point>> GetContours(const cv::Mat &img);
		std::vector<std::vector<cv::Point>> FilterSegmentsByGradients(std::vector<std::vector<cv::Point>> &contours);
		std::vector<std::vector<cv::Point>> SplitContourToSegmentsByGradients(double domOrientation, std::vector<cv::Point> &contour);
	};

}
#endif