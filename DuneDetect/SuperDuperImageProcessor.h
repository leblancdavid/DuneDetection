#pragma once

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class SuperDuperParams : public BaseImageProcessParameters
	{
	public:
		SuperDuperParams()
		{
			K = 7;
			DominantOrientationBins = 16;
		}
		~SuperDuperParams() {}
		SuperDuperParams(const SuperDuperParams &cpy)
		{
			K = cpy.K;
			DominantOrientationBins = cpy.DominantOrientationBins;
		}
		SuperDuperParams(int pK, double edgeThresh, int dominantOrientationBins)
		{
			K = pK;
			DominantOrientationBins = dominantOrientationBins;
		}

		int K;
		int DominantOrientationBins;

	};

	class SuperDuperImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		SuperDuperImageProcessor();
		~SuperDuperImageProcessor();
		SuperDuperImageProcessor(const SuperDuperImageProcessor &cpy);
		SuperDuperImageProcessor(SuperDuperParams *params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

		void SetParameters(BaseImageProcessParameters *params)
		{
			parameters = static_cast<SuperDuperParams*>(params);
		}

	private:

		void ComputeDirectionImage(cv::Mat &output, double dominantOrientation, const cv::Mat &scale = cv::Mat());
		void GetGradientImage(cv::Mat &output);
		SuperDuperParams *parameters;
	};



}