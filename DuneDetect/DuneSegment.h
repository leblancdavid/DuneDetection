#ifndef _DUNE_SEGMENT_H_
#define _DUNE_SEGMENT_H_

#include "OpenCVHeaders.h"
#include "BaseDuneFeatures.h"

namespace dune
{

struct DuneSegmentData
{
	cv::Point Position;
	double Orientation;
};

class DuneSegment : public BaseDuneFeature
{
public:
	DuneSegment() { }
	~DuneSegment() { }
	DuneSegment(const DuneSegment &cpy)
	{
		Data = cpy.Data;
	}

	std::vector<DuneSegmentData> Data;
private:

};

}
#endif