#pragma once

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class EdgeDirectionProcParams : public BaseImageProcessParameters
	{
	public:
		EdgeDirectionProcParams()
		{
			K = 7;
			DominantOrientationBins = 16;
		}
		~EdgeDirectionProcParams() {}
		EdgeDirectionProcParams(const EdgeDirectionProcParams &cpy)
		{
			K = cpy.K;
			DominantOrientationBins = cpy.DominantOrientationBins;
		}
		EdgeDirectionProcParams(int pK, double edgeThresh, int dominantOrientationBins)
		{
			K = pK;
			DominantOrientationBins = dominantOrientationBins;
		}

		int K;
		int DominantOrientationBins;

	};

	class EdgeDirectionImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		EdgeDirectionImageProcessor();
		~EdgeDirectionImageProcessor();
		EdgeDirectionImageProcessor(const EdgeDirectionImageProcessor &cpy);
		EdgeDirectionImageProcessor(EdgeDirectionProcParams *params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

		void SetParameters(BaseImageProcessParameters *params)
		{
			parameters = static_cast<EdgeDirectionProcParams*>(params);
		}

	private:

		void ComputeDirectionImage(cv::Mat &output, double dominantOrientation, const cv::Mat &scale = cv::Mat());
		EdgeDirectionProcParams *parameters;
	};



}