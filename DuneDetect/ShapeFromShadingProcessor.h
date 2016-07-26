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
		}
		~SFSProcessParameters() {}
		SFSProcessParameters(const SFSProcessParameters &cpy)
		{
			K = cpy.K;
			Iterations = cpy.Iterations;
		}
		SFSProcessParameters(int k, int it)
		{
			K = k;
			Iterations = it;
		}

		int Iterations;
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