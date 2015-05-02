#ifndef _EDGE_BASED_DUNE_DETECTOR_H_
#define _EDGE_BASED_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "EdgeDetectorImageProcessor.h"
#include "RotatedKernelEdgeDetector.h"

namespace dune
{

	class EdgeBasedDetectorParameters
	{
	public:
		EdgeBasedDetectorParameters()
		{
			K = 7;
			R = 0.5;
			MinSegmentLength = 10;
			ApplyLinking = false;
			LinkDistance = 40.0;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
		}
		EdgeBasedDetectorParameters(const EdgeBasedDetectorParameters &cpy)
		{
			K = cpy.K;
			R = cpy.R;
			ApplyLinking = cpy.ApplyLinking;
			LinkDistance = cpy.LinkDistance;
			MinSegmentLength = cpy.MinSegmentLength;
			HistogramBins = cpy.HistogramBins;
			AngleTolerance = cpy.AngleTolerance;
		}
		EdgeBasedDetectorParameters(int pK, double pR, int pMinSegmentLength, int pHistBins, double pAngleTolerance)
		{
			K = pK;
			R = pR;
			MinSegmentLength = pMinSegmentLength;
			HistogramBins = pHistBins;
			AngleTolerance = pAngleTolerance;
		}

		int K;
		double R;
		bool ApplyLinking;
		double LinkDistance;
		int MinSegmentLength;
		int HistogramBins;
		double AngleTolerance;

	};

	class EdgeBasedDuneDetector : public BaseDuneDetector
	{
	public:
		EdgeBasedDuneDetector();
		EdgeBasedDuneDetector(EdgeDetectorImageProcessor* imgproc, const EdgeBasedDetectorParameters &params);
		~EdgeBasedDuneDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

	private:

		EdgeBasedDetectorParameters Parameters;

		double GetDominantOrientation(const cv::Mat &inputImg, const cv::Mat &edges);
		cv::Mat FilterByDominantOrientationUsingKMeans(const cv::Mat &inputImg, const cv::Mat &edges);
		cv::Mat FilterByDominantOrientation(const cv::Mat &edges);

		std::vector<DuneSegment> GetDuneSegmentContours(const cv::Mat &img);
		std::vector<DuneSegment> FilterSegmentsByMagnitude(const std::vector<DuneSegment> &input);
		std::vector<DuneSegment> LinkDuneSegments(const std::vector<DuneSegment> &unlinked);
	};

}
#endif