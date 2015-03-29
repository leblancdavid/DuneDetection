#ifndef _WATERSHED_DUNE_DETECTOR_BENCHMARK_H_
#define _WATERSHED_DUNE_DETECTOR_BENCHMARK_H_

#include "BaseDuneDetectorBenchmarkTest.h"
#include "WatershedDuneDetector.h"

namespace dune
{

	class WatershedDuneDetectorBenchmark : public BaseDuneDetectorBenchmark
	{
	public:
		WatershedDuneDetectorBenchmark();
		WatershedDuneDetectorBenchmark(const WatershedDuneDetectorBenchmark &cpy);
		WatershedDuneDetectorBenchmark(const WatershedDuneDetectorParameters &params);
		~WatershedDuneDetectorBenchmark();

		void SetParams(const WatershedDuneDetectorParameters &params);

	private:
		WatershedDuneDetectorParameters DetectorParams;


	};

}

#endif