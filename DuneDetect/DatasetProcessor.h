#ifndef _PROCESS_DATASET_H_
#define _PROCESS_DATASET_H_

#include "DuneDetectLibHeaders.h"
#include <vector>

class DatasetProcessor
{
public:
	DatasetProcessor()
	{

	}

	~DatasetProcessor()
	{

	}

	void AddImageFile(std::string &path)
	{
		imageFiles.push_back(path);
	}

	void Process()
	{
		dune::AdaptiveImageProcParams params;
		params.BlockSize = 15;
		params.C = 0.5;
		params.Method = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
		params.ThresholdType = cv::THRESH_BINARY;
		dune::AdaptiveImageProcessor adaptiveProcessor(params);

		for(size_t i = 0; i < imageFiles.size(); ++i)
		{
			cv::Mat img = cv::imread(imageFiles[i],0);
			cv::Mat resultImg;
			adaptiveProcessor.Process(img, resultImg);

			cv::imshow("Processed Image: " + imageFiles[i], resultImg);
			cv::waitKey(0);
		}
	}

private:
	std::vector<std::string> imageFiles;
};

#endif