#ifndef _EDGE_BASED_DETECTOR_BENCHMARK_H_
#define _EDGE_BASED_DETECTOR_BENCHMARK_H_

#include "BaseDuneDetectorBenchmarkTest.h"
#include "EdgeBasedDuneDetector.h"

namespace dune
{

	class EdgeBasedDetectorBenchmark : public BaseDuneDetectorBenchmark
	{
	public:
		EdgeBasedDetectorBenchmark();
		EdgeBasedDetectorBenchmark(const EdgeBasedDetectorBenchmark &cpy);
		EdgeBasedDetectorBenchmark(EdgeBasedDetectorParameters *params);
		~EdgeBasedDetectorBenchmark();

		void SetParams(EdgeBasedDetectorParameters *params);

	private:
		EdgeBasedDetectorParameters *DetectorParams;

		
	};

}

#endif