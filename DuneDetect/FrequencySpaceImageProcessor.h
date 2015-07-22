#ifndef _FREQUENCY_SPACE_IMAGE_PROCESSOR_H_
#define _FREQUENCY_SPACE_IMAGE_PROCESSOR_H_

#include "BaseEdgeImageProcessor.h"
#include "DiscreteFourierTransform.h"

namespace dune
{
	//So far this class doesn't really have any significant parameters
	class FrequencySpaceProcParams : public BaseImageProcessParameters
	{
	public:
		FrequencySpaceProcParams()
		{

		}
		~FrequencySpaceProcParams() {}
		FrequencySpaceProcParams(const FrequencySpaceProcParams &cpy)
		{

		}
	};

	class FrequencySpaceImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		FrequencySpaceImageProcessor();
		~FrequencySpaceImageProcessor();
		FrequencySpaceImageProcessor(const FrequencySpaceImageProcessor &cpy);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

		void SetParameters(BaseImageProcessParameters *params)
		{
			Parameters = static_cast<FrequencySpaceProcParams*>(params);
		}

	private:

		imgproc::DiscreteFourierTransform DFTProcess;

		FrequencySpaceProcParams *Parameters;

		void ComputeDFT(const cv::Mat &inputImg, cv::Mat &outputImg);
		void FitEllipseToDFT(const cv::Mat &dftImg, cv::RotatedRect &elipse);
		void ApplyDFTFiltering(const cv::Mat &spectrum, cv::Mat &outputImg);
		void ApplyHighPassFilter(const cv::Mat &spectrum, cv::Mat &outputImg);
		void DirectionalSharpening(const cv::Mat &input, cv::Mat &output, double direction);

	};

}

#endif