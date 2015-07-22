#ifndef _WATERSHED_IMAGE_PROCESSOR_H_
#define _WATERSHED_IMAGE_PROCESSOR_H_

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class WatershedProcessorParameters : public BaseImageProcessParameters
	{
	public:
		WatershedProcessorParameters()
		{
			K = 3;
			HighQ = 1.2;
			LowQ =  1.2;
			HistogramBins = 16;
			AngleTolerance = 3.1416*0.5;
			Radius = 50;
		}

		~WatershedProcessorParameters() {}

		WatershedProcessorParameters(const WatershedProcessorParameters &cpy)
		{
			K = cpy.K;
			HighQ = cpy.HighQ;
			LowQ = cpy.LowQ;
			HistogramBins = cpy.HistogramBins;
			AngleTolerance = cpy.AngleTolerance;
			Radius = cpy.Radius;
		}
		WatershedProcessorParameters(int pK, double pHQ, double pLQ, int pHistBins, double pAngleTolerance, int pRadius)
		{
			K = pK;
			HighQ = pHQ;
			LowQ = pLQ;
			HistogramBins = pHistBins;
			AngleTolerance = pAngleTolerance;
			Radius = pRadius;
		}
		
		int K;
		double HighQ;
		double LowQ;
		int HistogramBins;
		double AngleTolerance;
		int Radius;
	};

	class WatershedImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		WatershedImageProcessor();
		~WatershedImageProcessor();
		WatershedImageProcessor(const WatershedImageProcessor &cpy);
		WatershedImageProcessor(WatershedProcessorParameters *params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

		void SetParameters(BaseImageProcessParameters *params)
		{
			Parameters = static_cast<WatershedProcessorParameters*>(params);
		}

	private:

		WatershedProcessorParameters *Parameters;

		void CannyBasedWatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg);
		void WatershedSegmentationIntensityBased(const cv::Mat &inputImg, cv::Mat &outputImg);
		void WatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg);

	};



}

#endif