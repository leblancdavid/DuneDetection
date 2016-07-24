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
		imgproc::ShapeFromShadingTsaiShah sfs;
		outputImg = sfs.Process(inputImg, cv::Vec2f(parameters->LightVector[0], parameters->LightVector[1]), parameters->Iterations);
	}
}