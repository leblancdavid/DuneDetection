#pragma once

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "EdgeDirectionImageProcessor.h"

namespace dune
{

	class EdgeDirectionDetectorParameters : public BaseDetectorParameters
	{
	public:
		EdgeDirectionDetectorParameters()
		{
			MinSegmentLength = 30;
			ImageProcessParameters = new EdgeDirectionProcParams();

		}
		EdgeDirectionDetectorParameters(const EdgeDirectionDetectorParameters &cpy)
		{
			ImageProcessParameters = cpy.ImageProcessParameters;
			MinSegmentLength = cpy.MinSegmentLength;
		}
		EdgeDirectionDetectorParameters(EdgeDirectionProcParams *procParams, int minSegmentLength)
		{
			ImageProcessParameters = procParams;
			MinSegmentLength = minSegmentLength;
		}

		int MinSegmentLength;

	};

	class EdgeDirectionDuneDetector : public BaseDuneDetector
	{
	public:
		EdgeDirectionDuneDetector();
		EdgeDirectionDuneDetector(EdgeDirectionImageProcessor* imgproc, EdgeDirectionDetectorParameters *params);
		~EdgeDirectionDuneDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

		void SetParameters(BaseDetectorParameters *params);

	private:

		EdgeDirectionDetectorParameters *Parameters;

		std::vector<DuneSegment> GetContourSegments(const cv::Mat &img);
		void FilterByEdgeDirection(const cv::Mat &input, std::vector<DuneSegment> &segments);
		std::vector<DuneSegment> FilterSegmentEdgeDirection(const DuneSegment &segments, const cv::Mat dx, const cv::Mat dy, int kSize);
		void ShiftSegmentsToGradientPeak(std::vector<DuneSegment> &segments, const cv::Mat &domMap);
		void ShiftSegmentsToGradientPeak2(std::vector<DuneSegment> &segments, const cv::Mat &domMap);
		void ShiftSegmentToOptimalGradientPeak(DuneSegment &segment, int kSize, double dir);
		bool Get2dShiftPeakAt(double x, double y, int k, double dir, cv::Vec2d &shift);
		bool Get2dShiftMagnitudeAt(double x, double y, int k, double dir, cv::Vec2d &shift);
		void SmoothShifts(std::vector<cv::Vec2d> &shift, int k);
	};

}