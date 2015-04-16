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
			for (size_t j = 0; j < dst.EndPoints.size(); ++i)
			{
				double d = std::sqrt((src.EndPoints[i].x - src.EndPoints[j].x) *
					(src.EndPoints[i].x - src.EndPoints[j].x) +
					(src.EndPoints[i].y - src.EndPoints[j].y) *
					(src.EndPoints[i].y - src.EndPoints[j].y));
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


	DuneSegment DuneSegment::Link(const DuneSegment &seg1, const DuneSegment &seg2, LinkingMethod method, LinkType type)
	{
		if (method == LinkingMethod::EndPointToEndPoint)
		{
			return LinkUsingEndPointToEndPoint(seg1, seg2, type);
		}
		else //For now the only linking method that is supported is endpoint to endpoint
		{
			return LinkUsingEndPointToEndPoint(seg1, seg2, type);
		}
	}

	void DuneSegment::ComputeEndPoints()
	{
		if (Segment.size() > 0)
		{
			for (size_t i = 0; i < Segment.size(); ++i)
			{
				//Find how many neighbors a segment has
				int neighbors = 0;
				for (size_t j = 0; j < Segment.size(); ++j)
				{
					//Don't compare it with itself
					if (i == j)
						continue;

					if (abs(Segment[i].Position.x - Segment[i].Position.x) <= 1 &&
						abs(Segment[i].Position.y - Segment[i].Position.y) <= 1)
					{
						neighbors++;
						//if a point has more than 1 neighbor then we can reject it as an endpoint
						if (neighbors > 1)
							break;
					}
				}
				//if it has 0 or 1 neighbors, it's an endpoint
				if (neighbors < 2)
				{
					EndPoints.push_back(Segment[i].Position);
				}
			}
		}
		
	}

	DuneSegment DuneSegment::LinkUsingEndPointToEndPoint(const DuneSegment &seg1, const DuneSegment &seg2, LinkType type)
	{
		int i1, i2;
		double dist = GetEndPointDistance(seg1, seg2, i1, i2);

		//Assign the seg1 segment data
		std::vector<DuneSegmentData> joinedSegment = seg1.GetSegmentData();
		std::vector<DuneSegmentData> sd2 = seg2.GetSegmentData();
		std::vector<cv::Point> ep1 = seg1.EndPoints;
		std::vector<cv::Point> ep2 = seg2.EndPoints;
		double xIncr = (double)(ep2[i2].x - ep1[i1].x) / dist;
		double yIncr = (double)(ep2[i2].y - ep1[i1].y) / dist;

		//Add the link between the two segments
		cv::Point2d pt = cv::Point2d((double)ep1[i1].x + xIncr, (double)ep1[i1].y + yIncr);
		cv::Point linkPt = cv::Point(std::ceil(pt.x - 0.5), std::ceil(pt.y - 0.5));
		while (linkPt != ep2[i2])
		{
			joinedSegment.push_back(DuneSegmentData(linkPt, 0));
			pt.x += xIncr;
			pt.y += yIncr;

			linkPt = cv::Point(std::ceil(pt.x - 0.5), std::ceil(pt.y - 0.5));
		}

		//Add the remaining points from the second segment
		for (size_t i = 0; i < sd2.size(); ++i)
		{
			joinedSegment.push_back(sd2[i]);
		}

		//Return the new segment
		return DuneSegment(joinedSegment);
	}
}