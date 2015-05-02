#ifndef _FREQUENCY_SPACE_IMAGE_PROCESSOR_H_
#define _FREQUENCY_SPACE_IMAGE_PROCESSOR_H_

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class FrequencySpaceImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		FrequencySpaceImageProcessor();
		~FrequencySpaceImageProcessor();
		FrequencySpaceImageProcessor(const FrequencySpaceImageProcessor &cpy);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

	private:

	};

}

#endif