#include "DuneSegmentDetectorBenchmark.h"

namespace dune
{

DuneSegmentDetectorBenchmark::DuneSegmentDetectorBenchmark()
{
	DetectorParams = new DuneSegmentDetectorParameters();
	Detector = new DuneSegmentDetector(new AdaptiveImageProcessor(), DetectorParams);
}

DuneSegmentDetectorBenchmark::DuneSegmentDetectorBenchmark(const DuneSegmentDetectorBenchmark &cpy)
{
	DetectorParams = cpy.DetectorParams;
}

DuneSegmentDetectorBenchmark::DuneSegmentDetectorBenchmark(DuneSegmentDetectorParameters *params)
{
	DetectorParams = params;
	Detector = new DuneSegmentDetector(new AdaptiveImageProcessor(), DetectorParams);
}

DuneSegmentDetectorBenchmark::~DuneSegmentDetectorBenchmark()
{

}

void DuneSegmentDetectorBenchmark::SetParams(DuneSegmentDetectorParameters *params)
{
	if(Detector == nullptr)
	{
		delete Detector;
	}
	DetectorParams = params;
	Detector = new DuneSegmentDetector(new AdaptiveImageProcessor(), DetectorParams);

}

}