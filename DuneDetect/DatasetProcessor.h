#ifndef _PROCESS_DATASET_H_
#define _PROCESS_DATASET_H_

#include "DuneDetectLibHeaders.h"

#include "DuneSegmentDetector.h"

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
		params.BlockSize = 101;
		params.C = -25.0;
		params.Method = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
		params.ThresholdType = cv::THRESH_BINARY;
		dune::BaseImageProcessor* adaptiveProcessor = new dune::AdaptiveImageProcessor(params);

		dune::DuneSegmentDetector detector;
		detector.SetImageProcess(adaptiveProcessor);

		for(size_t i = 0; i < imageFiles.size(); ++i)
		{
			cv::Mat img = cv::imread(imageFiles[i],0);
			
			std::vector<dune::DuneSegment> segments = detector.Extract(img);

			//cv::imwrite(imageFiles[i] + "_result.jpg", resultImg);
			//cv::imshow("Processed Image: " + imageFiles[i], resultImg);
			//cv::waitKey(0);
		}

		//delete adaptiveProcessor;
	}

private:
	std::vector<std::string> imageFiles;
};

#endif