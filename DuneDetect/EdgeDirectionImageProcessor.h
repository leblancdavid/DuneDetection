#pragma once

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class EdgeDirectionProcParams : public BaseImageProcessParameters
	{
	public:
		EdgeDirectionProcParams()
		{
			K = 9;
			DominantOrientationBins = 16;
			UseProvidedOrientation = true;
			Orientation = 0.0;
		}
		~EdgeDirectionProcParams() {}
		EdgeDirectionProcParams(const EdgeDirectionProcParams &cpy)
		{
			K = cpy.K;
			DominantOrientationBins = cpy.DominantOrientationBins;
			UseProvidedOrientation = cpy.UseProvidedOrientation;
			Orientation = cpy.Orientation;
		}
		EdgeDirectionProcParams(int pK, int dominantOrientationBins, bool useOrienation=false, double orientation=0.0)
		{
			K = pK;
			DominantOrientationBins = dominantOrientationBins;
			UseProvidedOrientation = useOrienation;
			Orientation = orientation;
		}

		int K;
		int DominantOrientationBins;
		bool UseProvidedOrientation;
		double Orientation;

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

		EdgeDirectionProcParams* GetParameters()
		{
			return parameters;
		}

	private:

		void ComputeDirectionImage(cv::Mat &output, double dominantOrientation, const cv::Mat &scale = cv::Mat());
		
		EdgeDirectionProcParams *parameters;
	};



}