#pragma once

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class SFSProcessParameters : public BaseImageProcessParameters
	{
	public:
		SFSProcessParameters()
		{
			Albedo = 1.0f;
			F = 100.0f;
			OpticalAxis.x = 0.0f;
			OpticalAxis.y = 0.0f;
			LightVector[0] = 0.0f;
			LightVector[1] = 0.0f;
			LightVector[2] = 0.0f;

			MaxDepth = 400.0f;
			Iterations = 12;
		}
		~SFSProcessParameters() {}
		SFSProcessParameters(const SFSProcessParameters &cpy)
		{
			Albedo = cpy.Albedo;
			F = cpy.F;
			OpticalAxis = cpy.OpticalAxis;
			LightVector = cpy.LightVector;
			MaxDepth = cpy.MaxDepth;
		}
		SFSProcessParameters(float albedo, float maxDepth, float f, const cv::Point2f &opticalAxis, const cv::Vec3f &light)
		{
			Albedo = albedo;
			F = f;
			OpticalAxis = opticalAxis;
			LightVector = light;
			MaxDepth = maxDepth;
		}

		float Albedo;
		float F;
		cv::Point2f OpticalAxis;
		cv::Vec3f LightVector;
		float MaxDepth;
		int Iterations;
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