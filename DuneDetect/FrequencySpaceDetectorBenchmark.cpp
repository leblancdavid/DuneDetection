#include "FrequencySpaceDetectorBenchmark.h"

namespace dune
{
	FrequencySpaceDetectorBenchmark::FrequencySpaceDetectorBenchmark()
	{
		Detector = new FrequencySpaceDuneDetector();
	}

	FrequencySpaceDetectorBenchmark::~FrequencySpaceDetectorBenchmark()
	{

	}
}