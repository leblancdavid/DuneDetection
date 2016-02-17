#ifndef _BASE_DUNE_DETECTOR_H_
#define _BASE_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseImageProcessor.h"
#include "DuneSegment.h"

namespace dune
{

	class BaseDetectorParameters
	{
	public:
		BaseDetectorParameters() {}
		BaseDetectorParameters(const BaseDetectorParameters &cpy) {}
		~BaseDetectorParameters() { delete ImageProcessParameters; }

		BaseImageProcessParameters *ImageProcessParameters;
	};

class BaseDuneDetector 
{
public:
	BaseDuneDetector() {};
	virtual ~BaseDuneDetector()
	{
		if (ImageProcess != NULL)
		{
			ImageProcess = NULL;
		}
		
	};
	BaseDuneDetector(const BaseDuneDetector &cpy){};

	virtual std::vector<DuneSegment> Extract(const cv::Mat &img) = 0;
	virtual void SetParameters(BaseDetectorParameters *parameters) = 0;

protected:

	BaseImageProcessor* ImageProcess;

};



}

#endif