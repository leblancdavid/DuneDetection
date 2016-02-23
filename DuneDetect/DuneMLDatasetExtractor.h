#ifndef _DUNE_ML_DATASET_EXTRACTOR_H_
#define _DUNE_ML_DATASET_EXTRACTOR_H_

#include "OpenCVHeaders.h"

namespace duneML
{
	class DuneMLDatasetParameters
	{
	public:
		DuneMLDatasetParameters() 
		{ 
			NumPositiveExamples = 1000;
			NumNegativeExamples = 1000;
			BufferDistancePixels = 5;
		}

		~DuneMLDatasetParameters() {}

		int NumPositiveExamples;
		int NumNegativeExamples;
		int BufferDistancePixels;
	};

	class DuneMLDatasetExtractor
	{
	public:
		DuneMLDatasetExtractor() {}
		DuneMLDatasetExtractor(const DuneMLDatasetParameters &params)
		{
			parameters = params;
		}
		DuneMLDatasetExtractor(const DuneMLDatasetExtractor &cpy)
		{
			parameters = cpy.parameters;
		}
		~DuneMLDatasetExtractor(){}

		void GetDatasetPoints(const cv::Mat &groundTruthImage,
			std::vector<cv::KeyPoint> &positive, std::vector<cv::KeyPoint> &negative);

	private:

		DuneMLDatasetParameters parameters;

	};
}


#endif