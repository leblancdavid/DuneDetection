#include "DuneSegment.h"

namespace dune
{
/*  ////////////////////////////////////////////////////////Dune Segment Data////////////////////////////////////////////////////////////////// */

	DuneSegmentData::DuneSegmentData()
	{
		Position.x = 0;
		Position.y = 0;
		Orientation = 0;
	}
	DuneSegmentData::~DuneSegmentData()
	{

	}
	DuneSegmentData::DuneSegmentData(const DuneSegmentData &cpy)
	{
		Position = cpy.Position;
		Orientation = cpy.Orientation;
	}
	DuneSegmentData::DuneSegmentData(cv::Point pos, double orient)
	{
		Position = pos;
		Orientation = orient;
	}

	double DuneSegmentData::DistanceFrom(cv::Point pos) const
	{
		double d = std::sqrt(((pos.x - Position.x) *
			(pos.x - Position.x)) +
			((pos.y - Position.y) *
			(pos.y - Position.y)));

		return d;
	}

/* ///////////////////////////////////////////////////////Dune Segments////////////////////////////////////////////////////////////////// */

	DuneSegment::DuneSegment() 
	{

	}
	DuneSegment::~DuneSegment() 
	{

	}

	DuneSegment::DuneSegment(const std::vector<DuneSegmentData> &segment)
	{
		Segment = segment;
		ComputeEndPoints();
	}

	DuneSegment::DuneSegment(const DuneSegment &cpy)
	{
		Segment = cpy.Segment;
		EndPoints = cpy.EndPoints;
	}

	cv::Point DuneSegment::FindClosestPoint(const cv::Point &from) const
	{
		int index;
		double dist;
		return FindClosestPoint(from, dist, index);
	}
	cv::Point DuneSegment::FindClosestPoint(const cv::Point &from, double &dist) const
	{
		int index;
		return FindClosestPoint(from, dist, index);
	}
	cv::Point DuneSegment::FindClosestPoint(const cv::Point &from, double &dist, int &index) const
	{
		dist = DBL_MAX;
		cv::Point closest;
		for (size_t i = 0; i < Segment.size(); ++i)
		{
			//Compute Euclidean distance
			double d = Segment[i].DistanceFrom(from);

			if (d < dist)
			{
				dist = d;
				index = i;
				closest = Segment[i].Position;
			}
		}

		return closest;
	}

	double DuneSegment::GetEndPointDistance(const DuneSegment &src, const DuneSegment &dst, int &srcIndex, int &dstIndex)
	{
		//The goal of this function is to find the closest endpoints between the src and dst segments.
		double minDist = DBL_MAX;
		for (size_t i = 0; i < src.EndPoints.size(); ++i)
		{
			for (size_t j = 0; j < dst.EndPoints.size(); ++j)
			{
				double d = std::sqrt((src.EndPoints[i].x - dst.EndPoints[j].x) *
					(src.EndPoints[i].x - dst.EndPoints[j].x) +
					(src.EndPoints[i].y - dst.EndPoints[j].y) *
					(src.EndPoints[i].y - dst.EndPoints[j].y));
				if (d < minDist)
				{
					minDist = d;
					srcIndex = (int)i;
					dstIndex = (int)j;
				}
			}
		}

		return minDist;
	}

	double DuneSegment::GetEndPointDistance(const DuneSegment &src, const DuneSegment &dst)
	{
		int srcIndex, dstIndex;
		return GetEndPointDistance(src, dst, srcIndex, dstIndex);
	}

	void DuneSegment::ComputeEndPoints()
	{
		if (Segment.size() > 0)
		{
			for (size_t i = 0; i < Segment.size(); ++i)
			{
				//Find how many neighbors a segment has
				int neighbors = 0;
				std::vector<cv::Point> n;
				for (size_t j = 0; j < Segment.size(); ++j)
				{
					//Don't compare it with itself
					if (i == j)
						continue;

					if (abs(Segment[i].Position.x - Segment[j].Position.x) <= 1 &&
						abs(Segment[i].Position.y - Segment[j].Position.y) <= 1)
					{
						neighbors++;
						n.push_back(Segment[j].Position);
						//if a point has more than 2 neighbor then we can reject it as an endpoint
						if (neighbors > 2)
							break;
					}
				}
				//if it has 0 or 1 neighbors, it's an endpoint
				if (neighbors < 2)
				{
					EndPoints.push_back(Segment[i].Position);
				}
				else if (neighbors == 2)
				{
					int d = std::abs(n[0].x - n[1].x) + std::abs(n[0].y - n[1].y);
					if (d <= 1)
						EndPoints.push_back(Segment[i].Position);
				}
			}
		}
		
	}
}