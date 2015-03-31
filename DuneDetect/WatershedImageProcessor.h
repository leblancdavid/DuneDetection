#ifndef _WATERSHED_IMAGE_PROCESSOR_H_
#define _WATERSHED_IMAGE_PROCESSOR_H_

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class WatershedProcessorParameters
	{
	public:
		WatershedProcessorParameters()
		{
			K = 7;
			HighQ = 1.0;
			LowQ = 0.5;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
		}

		~WatershedProcessorParameters() {}

		WatershedProcessorParameters(const WatershedProcessorParameters &cpy)
		{
			K = cpy.K;
			HighQ = cpy.HighQ;
			LowQ = cpy.LowQ;
			HistogramBins = cpy.HistogramBins;
			AngleTolerance = cpy.AngleTolerance;
		}
		WatershedProcessorParameters(int pK, double pHQ, double pLQ, int pHistBins, double pAngleTolerance)
		{
			K = pK;
			HighQ = pHQ;
			LowQ = pLQ;
			HistogramBins = pHistBins;
			AngleTolerance = pAngleTolerance;
		}
		
		int K;
		double HighQ;
		double LowQ;
		int HistogramBins;
		double AngleTolerance;
	};

	class WatershedImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		WatershedImageProcessor();
		~WatershedImageProcessor();
		WatershedImageProcessor(const WatershedImageProcessor &cpy);
		WatershedImageProcessor(const WatershedProcessorParameters &params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

	private:

		WatershedProcessorParameters Parameters;

		void WatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg);
		std::vector<double> FindDominantOrientations();

	};



}

#endif