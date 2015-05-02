#ifndef _FREQUENCY_SPACE_DUNE_DETECTOR_H_
#define _FREQUENCY_SPACE_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"

namespace dune
{
	class FrequencySpaceDuneDetector : public BaseDuneDetector
	{
	public:
		FrequencySpaceDuneDetector();
		~FrequencySpaceDuneDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

	private:

	};

}
#endif