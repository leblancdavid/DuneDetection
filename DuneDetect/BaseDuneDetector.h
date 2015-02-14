#ifndef _BASE_DUNE_DETECTOR_H_
#define _BASE_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseImageProcessor.h"

namespace dune
{

struct DuneSegmentData
{
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
			double d = std::sqrt(((from.x-Data[i].Position.x) *
								(from.x-Data[i].Position.x)) +
								((from.y-Data[i].Position.y) *
								(from.y-Data[i].Position.y)));
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
	
	void SetImageProcess(BaseImageProcessor *imgproc)
	{
		ImageProcess = imgproc;
	}

	virtual std::vector<DuneSegment> Extract(const cv::Mat &img)=0;

protected:

	BaseImageProcessor* ImageProcess;

};



}

#endif