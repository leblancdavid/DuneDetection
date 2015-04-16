#ifndef _BASE_DUNE_DETECTOR_H_
#define _BASE_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseImageProcessor.h"
#include "DuneSegment.h"

namespace dune
{

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