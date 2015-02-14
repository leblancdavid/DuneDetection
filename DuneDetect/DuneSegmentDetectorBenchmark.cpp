#include "DuneSegmentDetectorBenchmark.h"

namespace dune
{

DuneSegmentDetectorBenchmark::DuneSegmentDetectorBenchmark()
{
	DetectorParams = DuneSegmentDetectorParameters();
	Detector = new DuneSegmentDetector(new AdaptiveImageProcessor(), DetectorParams);
}

DuneSegmentDetectorBenchmark::DuneSegmentDetectorBenchmark(const DuneSegmentDetectorBenchmark &cpy)
{
	DetectorParams = cpy.DetectorParams;
}

DuneSegmentDetectorBenchmark::DuneSegmentDetectorBenchmark(const DuneSegmentDetectorParameters &params)
{
	Detector = new DuneSegmentDetector(new AdaptiveImageProcessor(), DetectorParams);
}

DuneSegmentDetectorBenchmark::~DuneSegmentDetectorBenchmark()
{

}

void DuneSegmentDetectorBenchmark::SetParams(const DuneSegmentDetectorParameters &params)
{
	if(Detector == nullptr)
	{
		delete Detector;
	}
	DetectorParams = params;
	Detector = new DuneSegmentDetector(new AdaptiveImageProcessor(), DetectorParams);

}

}