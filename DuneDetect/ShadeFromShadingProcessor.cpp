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
		cv::Mat P, Q;

		/*imgproc::ShapeFromShadingTsaiShah sfs;
		outputImg = sfs.Process(inputImg, parameters->Iterations, parameters->K, P, Q, parameters->MaskThreshold, parameters->ErosionIterations);

		cv::Mat normP, normQ;
		cv::normalize(P, normP, 0.0, 1.0, cv::NORM_MINMAX);
		cv::imshow("normP", normP);
		cv::normalize(Q, normQ, 0.0, 1.0, cv::NORM_MINMAX);
		cv::imshow("normQ", normQ);
		cv::waitKey(0);*/
		//cv::normalize(Z, Z, 0.0, 1.0, cv::NORM_MINMAX);
		
		//Recover the depth
		//outputImg = imgproc::DepthFromGradients(P, Q);

		//cv::Mat roi;
		//roi = inputImg(cv::Rect(400, 400, 100, 100));
		//cv::imshow("roi", roi);
		//cv::waitKey(330);
		imgproc::ShapeFromShadingGradient sfs;
		cv::Vec3f illumination;
		
		outputImg = sfs.Process(inputImg, parameters->K, parameters->Illumination, P, Q);

	}
}