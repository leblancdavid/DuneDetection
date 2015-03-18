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
		EdgeBasedDetectorBenchmark(const EdgeBasedDetectorParameters &params);
		~EdgeBasedDetectorBenchmark();

		void SetParams(const EdgeBasedDetectorParameters &params);

	private:
		EdgeBasedDetectorParameters DetectorParams;

		
	};

}

#endif