#ifdef EDGE_BASED_DETECTOR_WRAPPER_EXPORTS
#define EDGE_BASED_DETECTOR_WRAPPER_DLL_API __declspec(dllexport) 
#else
#define EDGE_BASED_DETECTOR_WRAPPER_DLL_API __declspec(dllimport) 
#endif

#ifndef _EDGE_BASED_DETECTOR_WRAPPER_H_
#define _EDGE_BASED_DETECTOR_WRAPPER_H_

namespace detectors
{
	class EdgeBasedDetectorParameters
	{
	public:
		EdgeBasedDetectorParameters()
		{
			K = 5;
			MinSegmentLength = 10;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
		}
		~EdgeBasedDetectorParameters() {}

		int K;
		int MinSegmentLength;
		int HistogramBins;
		double AngleTolerance;
	};

	class EdgeBasedDetectorWrapper
	{
	public:
		EdgeBasedDetectorWrapper();
		~EdgeBasedDetectorWrapper();


	};
}

#endif