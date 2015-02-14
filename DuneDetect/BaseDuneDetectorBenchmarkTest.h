#ifndef _DUNE_DETECTOR_BENCHMARK_TEST_H_
#define _DUNE_DETECTOR_BENCHMARK_TEST_H_

#include "BaseDuneDetector.h"

namespace dune
{

class BenchmarkResults
{
public:
	BenchmarkResults()
	{
		TP = 0;
		FP = 0;
		TN = 0;
		FN = 0;
		Error = 0;
	}
	BenchmarkResults(const BenchmarkResults &cpy)
	{
		TP = cpy.TP;
		FP = cpy.FP;
		TN = cpy.TN;
		FN = cpy.FN;
		Error = cpy.Error;
	}

	~BenchmarkResults() {}


	double TP;
	double FP;
	double TN;
	double FN;
	double Error;

private:
};

class BenchmarkTestParameters
{
public:
	double MinError;

private:
};

class BaseDuneDetectorBenchmark
{
public:
	BaseDuneDetectorBenchmark() 
	{

	}
	~BaseDuneDetectorBenchmark()
	{
		delete Detector;
	}
	
	BenchmarkResults GetResults(const std::string &testImageFile,
								const std::string &groundTruthImageFile)
	{
		cv::Mat testImg = cv::imread(testImageFile, 0);
		cv::Mat groundTruthImg = cv::imread(groundTruthImageFile, 0);

		std::vector<DuneSegment> segments = Detector->Extract(testImg);
		std::vector<cv::Point> groundTruth = GetGroundTruthPoints(groundTruthImg);

		return GetBenchmarkResults(segments, groundTruth);
	}

protected:
	BaseDuneDetector *Detector;
	BenchmarkTestParameters BenchmarkParams;

	std::vector<cv::Point> GetGroundTruthPoints(const cv::Mat &groundTruthImage)
	{
		std::vector<cv::Point> groundTruthLocations;
		for(int y = 0; y < groundTruthImage.rows; ++y)
		{
			for(int x = 0; x < groundTruthImage.cols; ++x)
			{
				if(groundTruthImage.at<uchar>(y,x) != 0)
				{
					groundTruthLocations.push_back(cv::Point(x,y));
				}
			
			}
		}

		return groundTruthLocations;
	}

	BenchmarkResults GetBenchmarkResults(const std::vector<DuneSegment> &segments,
											const std::vector<cv::Point> &groundTruth)
	{
		BenchmarkResults results;

		//Compute the true positives;
		for(size_t i = 0; i < groundTruth.size(); ++i)
		{
			int minJ;
			double minDist = DBL_MAX;

			for(size_t j = 0; j < segments.size(); ++j)
			{
				double d;
				cv::Point closest = segments[i].FindClosestPoint(groundTruth[i], d);
				if(d < minDist)
				{
					minJ = j;
					minDist = d;
				}
			}

			if(minDist < BenchmarkParams.MinError)
			{

			}
		}

		return results;
	}

};

}

#endif