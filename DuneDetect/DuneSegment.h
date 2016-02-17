#ifndef _BASE_DUNE_SEGMENT_H_
#define _BASE_DUNE_SEGMENT_H_

#include "OpenCVHeaders.h"

namespace dune
{
	class DuneSegmentData
	{
	public:
		DuneSegmentData();
		~DuneSegmentData();
		DuneSegmentData(const DuneSegmentData &cpy);
		DuneSegmentData(cv::Point pos, double orient);
		double DistanceFrom(cv::Point pos) const;

		cv::Point Position;
		double Orientation;
	};

	class DuneSegment
	{
	public:
		DuneSegment();
		~DuneSegment();
		DuneSegment(const std::vector<DuneSegmentData> &segment);
		DuneSegment(const DuneSegment &cpy);

		cv::Point FindClosestPoint(const cv::Point &from) const;
		cv::Point FindClosestPoint(const cv::Point &from, double &dist) const;
		cv::Point FindClosestPoint(const cv::Point &from, double &dist, int &index) const;

		static double GetEndPointDistance(const DuneSegment &src, const DuneSegment &dst);
		static double GetEndPointDistance(const DuneSegment &src, const DuneSegment &dst, int &srcIndex, int &dstIndex);
		static double GetLineDistance(const DuneSegment &src, const DuneSegment &dst);

		DuneSegmentData operator [] (int i) const { return Segment[i]; }
		DuneSegmentData& operator [] (int i) { return Segment[i]; }

		std::vector<cv::Point> GetEndPoints() const
		{
			return EndPoints;
		}

		std::vector<DuneSegmentData> GetSegmentData() const
		{
			return Segment;
		}

		int GetLength() const
		{
			return (int)Segment.size();
		}

		cv::Vec4d GetLine() const
		{
			return Line;
		}

		void SetSegmentData(const std::vector<DuneSegmentData> &segData)
		{
			Segment = segData;
			ComputeEndPoints();
		}

		void ComputeFeatures();
		
	private:

		std::vector<DuneSegmentData> Segment;		
		std::vector<cv::Point> EndPoints;
		cv::Vec4f Line;

		void ComputeEndPoints();
		void FitLine();
		void Validate();
		
	};

}




#endif