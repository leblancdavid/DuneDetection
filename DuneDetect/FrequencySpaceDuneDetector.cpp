#include "FrequencySpaceDuneDetector.h"
#include "FrequencySpaceImageProcessor.h"
namespace dune
{
	FrequencySpaceDuneDetector::FrequencySpaceDuneDetector()
	{
		ImageProcess = new FrequencySpaceImageProcessor();
	}

	FrequencySpaceDuneDetector::~FrequencySpaceDuneDetector()
	{

	}

	std::vector<DuneSegment> FrequencySpaceDuneDetector::Extract(const cv::Mat &img)
	{
		cv::Mat processedImg;
		ImageProcess->Process(img, processedImg);

		std::vector<DuneSegment> segments;

		return segments;
	}
}