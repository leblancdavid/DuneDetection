#include "EdgeBasedDetectorBenchmark.h"

namespace dune
{

EdgeBasedDetectorBenchmark::EdgeBasedDetectorBenchmark()
{
	DetectorParams = EdgeBasedDetectorParameters();
	Detector = new EdgeBasedDuneDetector();
}

EdgeBasedDetectorBenchmark::EdgeBasedDetectorBenchmark(const EdgeBasedDetectorBenchmark &cpy)
{
	DetectorParams = cpy.DetectorParams;
}

EdgeBasedDetectorBenchmark::EdgeBasedDetectorBenchmark(const EdgeBasedDetectorParameters &params)
{
	Detector = new EdgeBasedDuneDetector();
}

EdgeBasedDetectorBenchmark::~EdgeBasedDetectorBenchmark()
{

}

void EdgeBasedDetectorBenchmark::SetParams(const EdgeBasedDetectorParameters &params)
{
	if (Detector == nullptr)
	{
		delete Detector;
	}
	DetectorParams = params;
	Detector = new EdgeBasedDuneDetector(new EdgeDetectorImageProcessor(), DetectorParams);

}

}