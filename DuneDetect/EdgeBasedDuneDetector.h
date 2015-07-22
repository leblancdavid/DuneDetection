#ifndef _EDGE_BASED_DUNE_DETECTOR_H_
#define _EDGE_BASED_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "EdgeDetectorImageProcessor.h"
#include "RotatedKernelEdgeDetector.h"

namespace dune
{

	class EdgeBasedDetectorParameters : public BaseDetectorParameters
	{
	public:
		EdgeBasedDetectorParameters()
		{
			R = 0.0;
			MinSegmentLength = 50;
			ImageProcessParameters = new EdgeDetectorProcParams();

			//Not in use
			K = 7;
			ApplyLinking = false;
			LinkDistance = 40.0;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
		}
		EdgeBasedDetectorParameters(const EdgeBasedDetectorParameters &cpy)
		{
			R = cpy.R;
			ImageProcessParameters = cpy.ImageProcessParameters;
			MinSegmentLength = cpy.MinSegmentLength;

			//Not in use
			K = cpy.K;
			ApplyLinking = cpy.ApplyLinking;
			LinkDistance = cpy.LinkDistance;
			HistogramBins = cpy.HistogramBins;
			AngleTolerance = cpy.AngleTolerance;
		}
		EdgeBasedDetectorParameters(EdgeDetectorProcParams *procParams, double r, int minSegmentLength)
		{
			ImageProcessParameters = procParams;
			R = r;
			MinSegmentLength = minSegmentLength;

			//Not in use
			K = 7;
			ApplyLinking = false;
			LinkDistance = 40.0;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
		}

		double R;
		int MinSegmentLength;

		int K;
		bool ApplyLinking;
		double LinkDistance;
		int HistogramBins;
		double AngleTolerance;

	};

	class EdgeBasedDuneDetector : public BaseDuneDetector
	{
	public:
		EdgeBasedDuneDetector();
		EdgeBasedDuneDetector(EdgeDetectorImageProcessor* imgproc, EdgeBasedDetectorParameters *params);
		~EdgeBasedDuneDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

		void SetParameters(BaseDetectorParameters *params);

	private:

		EdgeBasedDetectorParameters *Parameters;

		std::vector<DuneSegment> GetContourSegments(const cv::Mat &img);
		double GetDominantOrientation(const cv::Mat &inputImg, const cv::Mat &edges);
		cv::Mat FilterByDominantOrientationUsingKMeans(const cv::Mat &inputImg, const cv::Mat &edges);
		cv::Mat FilterByDominantOrientation(const cv::Mat &edges);

		std::vector<DuneSegment> GetDuneSegments(const cv::Mat &img);
		std::vector<DuneSegment> FilterSegmentsByMagnitude(const std::vector<DuneSegment> &input);
		std::vector<DuneSegment> FilterSegmentsByLineOrientation(const std::vector<DuneSegment> &input);
		std::vector<DuneSegment> FilterSegmentsByIntensityValues(const std::vector<DuneSegment> &input, const cv::Mat &img);
		
		std::vector<DuneSegment> SplitSegmentByIntensity(const DuneSegment &input, const cv::Mat &img, double threshold, int neighbors);

		void ComputeSegmentLines(std::vector<DuneSegment> &segments);
		std::vector<double> ComputeLineOrientationHistogram(std::vector<DuneSegment> &input);
		
		std::vector<DuneSegment> LinkDuneSegments(const std::vector<DuneSegment> &unlinked);


	};

}
#endif