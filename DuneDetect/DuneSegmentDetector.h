#ifndef _DUNE_SEGMENT_DETECTOR_H_
#define _DUNE_SEGMENT_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "DuneSegment.h"

namespace dune
{

class DuneSegmentDetector : public BaseDuneDetector<DuneSegment>
{
public:
	DuneSegmentDetector() { }
	~DuneSegmentDetector() { }
	DuneSegmentDetector(const DuneSegment &cpy)
	{
	}

	std::vector<DuneSegment> Extract(const cv::Mat &img)
	{
		std::vector<DuneSegment> duneSegs;

		return duneSegs;
	}

private:

};

}
#endif