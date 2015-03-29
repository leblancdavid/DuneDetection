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

		}

		~WatershedProcessorParameters() {}

		WatershedProcessorParameters(const WatershedProcessorParameters &cpy)
		{

		}
		WatershedProcessorParameters(int k)
		{

		}

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

		WatershedProcessorParameters parameters;

		void WatershedSegmentation(const cv::Mat &inputImg, cv::Mat &outputImg);

	};



}

#endif