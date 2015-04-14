#ifndef _BASE_DUNE_DETECTOR_H_
#define _BASE_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseImageProcessor.h"

namespace dune
{

class DuneSegmentData
{
public:
	DuneSegmentData()
	{
		Position.x = 0;
		Position.y = 0;
		Orientation = 0;
	}
	~DuneSegmentData(){}
	DuneSegmentData(const DuneSegmentData &cpy)
	{
		Position = cpy.Position;
		Orientation = cpy.Orientation;
	}
	DuneSegmentData(cv::Point pos, double orient)
	{
		Position = pos;
		Orientation = orient;
	}

	double DistanceFrom(cv::Point pos) const
	{
		double d = std::sqrt(((pos.x - Position.x) *
			(pos.x - Position.x)) +
			((pos.y - Position.y) *
			(pos.y - Position.y)));

		return d;
	}

	cv::Point Position;
	double Orientation;
};

class DuneSegment
{
public:
	DuneSegment() { }
	~DuneSegment() { }
	DuneSegment(const DuneSegment &cpy)
	{
		Data = cpy.Data;
	}

	cv::Point FindClosestPoint(const cv::Point &from) const
	{
		int index;
		double dist;
		return FindClosestPoint(from, dist, index);
	}
	cv::Point FindClosestPoint(const cv::Point &from, double &dist) const
	{
		int index;
		return FindClosestPoint(from, dist, index);
	}
	cv::Point FindClosestPoint(const cv::Point &from, double &dist, int &index) const
	{
		dist = DBL_MAX;
		cv::Point closest;
		for(size_t i = 0; i < Data.size(); ++i)
		{
			//Compute Euclidean distance
			double d = Data[i].DistanceFrom(from);

			if(d < dist)
			{
				dist = d;
				index = i;
				closest = Data[i].Position;
			}
		}

		return closest;
	}

	std::vector<DuneSegmentData> Data;
private:

};

class BaseDuneDetector 
{
public:
	BaseDuneDetector() {};
	~BaseDuneDetector() 
	{
		delete ImageProcess;
	};
	BaseDuneDetector(const BaseDuneDetector &cpy){};

	virtual std::vector<DuneSegment> Extract(const cv::Mat &img)=0;

protected:

	BaseImageProcessor* ImageProcess;

};



}

#endif