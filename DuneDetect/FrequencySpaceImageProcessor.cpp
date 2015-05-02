#include "FrequencySpaceImageProcessor.h"

namespace dune
{
	FrequencySpaceImageProcessor::FrequencySpaceImageProcessor()
	{


	}

	FrequencySpaceImageProcessor::~FrequencySpaceImageProcessor()
	{

	}

	FrequencySpaceImageProcessor::FrequencySpaceImageProcessor(const FrequencySpaceImageProcessor &cpy)
	{

	}

	void FrequencySpaceImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		cv::Mat dftImg;
		cv::dft(inputImg, dftImg);

		outputImg = inputImg.clone();
	}

}