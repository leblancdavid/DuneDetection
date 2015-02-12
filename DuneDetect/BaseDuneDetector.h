#ifndef _BASE_DUNE_DETECTOR_H_
#define _BASE_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseImageProcessor.h"

namespace dune
{

template <class T>
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

	virtual std::vector<T> Extract(const cv::Mat &img)=0;

protected:

	BaseImageProcessor* ImageProcess;

};



}

#endif