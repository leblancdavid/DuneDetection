#pragma once

#include "BaseEdgeImageProcessor.h"
#include "BaseDuneDetector.h"

namespace dune
{
	class SFSProcessParameters : public BaseImageProcessParameters
	{
	public:
		SFSProcessParameters()
		{
			K = 7;
			Iterations = 20;
			MaskThreshold = 0;
			ErosionIterations = 0;
		}
		~SFSProcessParameters() {}
		SFSProcessParameters(const SFSProcessParameters &cpy)
		{
			K = cpy.K;
			Iterations = cpy.Iterations;
			MaskThreshold = cpy.MaskThreshold;
			ErosionIterations = cpy.ErosionIterations;
			Illumination = cpy.Illumination;
		}
		SFSProcessParameters(int k, int it, int mask, int erode, const cv::Vec3f &illumination)
		{
			K = k;
			Iterations = it;
			MaskThreshold = mask;
			ErosionIterations = erode;
			Illumination = illumination;
		}

		int Iterations;
		int MaskThreshold;
		int ErosionIterations;
		cv::Vec3f Illumination;
		int K;
	};

	class SFSImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		SFSImageProcessor();
		~SFSImageProcessor();
		SFSImageProcessor(const SFSImageProcessor &cpy);
		SFSImageProcessor(SFSProcessParameters *params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

		void SetParameters(BaseImageProcessParameters *params)
		{
			parameters = static_cast<SFSProcessParameters*>(params);
		}

		SFSProcessParameters* GetParameters()
		{
			return parameters;
		}

	private:


		SFSProcessParameters *parameters;
	};



}