#include "WatershedDuneDetectorBenchmark.h"

namespace dune
{
WatershedDuneDetectorBenchmark::WatershedDuneDetectorBenchmark()
{
	DetectorParams = WatershedDuneDetectorParameters();
	Detector = new WatershedDuneDetector();
}

WatershedDuneDetectorBenchmark::WatershedDuneDetectorBenchmark(const WatershedDuneDetectorBenchmark &cpy)
{
	DetectorParams = cpy.DetectorParams;
}

WatershedDuneDetectorBenchmark::WatershedDuneDetectorBenchmark(const WatershedDuneDetectorParameters &params)
{
	Detector = new WatershedDuneDetector();
}

WatershedDuneDetectorBenchmark::~WatershedDuneDetectorBenchmark()
{

}

void WatershedDuneDetectorBenchmark::SetParams(const WatershedDuneDetectorParameters &params)
{
	if (Detector == nullptr)
	{
		delete Detector;
	}
	DetectorParams = params;

	Detector = new WatershedDuneDetector(new WatershedImageProcessor(), DetectorParams);
}

}