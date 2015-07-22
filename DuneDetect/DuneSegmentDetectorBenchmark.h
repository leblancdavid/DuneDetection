#ifndef _DUNE_SEGMENT_DETECTOR_BENCHMARK_H_
#define _DUNE_SEGMENT_DETECTOR_BENCHMARK_H_

#include "BaseDuneDetectorBenchmarkTest.h"
#include "DuneSegmentDetector.h"

namespace dune
{

class DuneSegmentDetectorBenchmark : public BaseDuneDetectorBenchmark
{
public:
	DuneSegmentDetectorBenchmark();
	DuneSegmentDetectorBenchmark(const DuneSegmentDetectorBenchmark &cpy);
	DuneSegmentDetectorBenchmark(DuneSegmentDetectorParameters *params);
	~DuneSegmentDetectorBenchmark();

	void SetParams(DuneSegmentDetectorParameters *params);

private:
	DuneSegmentDetectorParameters *DetectorParams;

};

}

#endif