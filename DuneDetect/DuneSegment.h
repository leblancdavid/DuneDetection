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

		enum LinkingMethod { EndPointToEndPoint, EndPointToNearest };
		enum LinkType { Linear, Poly, Cubic };
		static DuneSegment Link(const DuneSegment &seg1, const DuneSegment &seg2, LinkingMethod method, LinkType type);

		std::vector<cv::Point> GetEndPoints() const
		{
			return EndPoints;
		}

		std::vector<DuneSegmentData> GetSegmentData() const
		{
			return Segment;
		}

		void SetSegmentData(const std::vector<DuneSegmentData> &segData)
		{
			Segment = segData;
			ComputeEndPoints();
		}
		
	private:

		std::vector<DuneSegmentData> Segment;		
		std::vector<cv::Point> EndPoints;

		void ComputeEndPoints();
		static DuneSegment LinkUsingEndPointToEndPoint(const DuneSegment &seg1, const DuneSegment &seg2, LinkType type);
	};

}




#endif