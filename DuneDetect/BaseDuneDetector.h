#ifndef _DUNE_DETECTOR_H_
#define _DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneFeatures.h"
#include "ImageProcessor.h"


namespace dune
{

template <class T>
class BaseDuneDetector 
{
public:
	BaseDuneDetector() {};
	~BaseDuneDetector() {};
	BaseDuneDetector(const BaseDuneDetector &cpy){};

	virtual std::vector<T> Extract(const cv::Mat &img)=0;

private:
};

}

#endif