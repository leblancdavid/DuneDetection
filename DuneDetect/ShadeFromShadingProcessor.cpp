#include "ShapeFromShadingProcessor.h"

#include "ShapeFromShading.h"

namespace dune
{
	SFSImageProcessor::SFSImageProcessor()
	{
		parameters = new SFSProcessParameters();
	}

	SFSImageProcessor::~SFSImageProcessor()
	{
	}

	SFSImageProcessor::SFSImageProcessor(const SFSImageProcessor &cpy)
	{
		parameters = cpy.parameters;
	}

	SFSImageProcessor::SFSImageProcessor(SFSProcessParameters *params)
	{
		parameters = params;
	}

	void SFSImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		//imgproc::ShapeFromShadingTsaiShah sfs;
		//outputImg = sfs.Process(inputImg, parameters->Iterations, parameters->K);

		imgproc::ShapeFromShadingGradient sfs;
		cv::Mat P, Q;
		
		P *= -1.0f;
		Q *= -1.0f;
		outputImg = sfs.Process(inputImg, parameters->K, P, Q);
	}
}