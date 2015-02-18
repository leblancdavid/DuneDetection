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
	BenchmarkTestParameters()
	{
		MinError = 0;
	}
	~BenchmarkTestParameters(){}
	BenchmarkTestParameters(const BenchmarkTestParameters &cpy)
	{
		MinError = cpy.MinError;
	}
	BenchmarkTestParameters(double pMinError)
	{
		MinError = pMinError;
	}

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
	BaseDuneDetectorBenchmark(const BaseDuneDetectorBenchmark &cpy)
	{
		BenchmarkParams = cpy.BenchmarkParams;
	}
	BaseDuneDetectorBenchmark(const BenchmarkTestParameters &params)
	{
		BenchmarkParams = params;
	}

	BenchmarkTestParameters BenchmarkParams;

	BenchmarkResults GetResults(const std::string &testImageFile,
								const std::string &groundTruthImageFile)
	{
		cv::Mat testImg = cv::imread(testImageFile, 0);
		cv::Mat groundTruthImg = cv::imread(groundTruthImageFile, 0);

		std::vector<DuneSegment> segments = Detector->Extract(testImg);
		std::vector<cv::Point> groundTruth = GetGroundTruthPoints(groundTruthImg);

		cv::Mat colorImg;// = cv::Mat::zeros(groundTruthImg.rows, groundTruthImg.cols, CV_8UC3);
		cv::cvtColor(testImg, colorImg, CV_GRAY2BGR);
		for(size_t i = 0; i < segments.size(); ++i)
		{
			for(size_t j = 0; j < segments[i].Data.size(); ++j)
			{
				cv::Point p = segments[i].Data[j].Position;
				colorImg.at<cv::Vec3b>(p) = cv::Vec3b(0,0,255);
			}
		}

		for(size_t i = 0; i < groundTruth.size(); ++i)
		{
			colorImg.at<cv::Vec3b>(groundTruth[i])[1] = 255;// = cv::Vec3b(0,255,0);
		}
		cv::imshow("Ground Truth Image", colorImg);
		cv::waitKey(0);

		BenchmarkResults results;
		//results = GetBenchmarkResults(segments, groundTruth);
		return results;
	}

protected:
	BaseDuneDetector *Detector;
	

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

		results.TP = CalcTruePositives(segments, groundTruth, results.Error);
		results.FP = CalcFalsePositives(segments, groundTruth);
		
		return results;
	}

	double CalcTruePositives(const std::vector<DuneSegment> &segments,
									const std::vector<cv::Point> &groundTruth,
									double &error)
	{
		double TP = 0;
		error = 0;
		//Compute the true positives;
		for(size_t i = 0; i < groundTruth.size(); ++i)
		{
			int minJ;
			double minDist = DBL_MAX;

			for(size_t j = 0; j < segments.size(); ++j)
			{
				double d;
				cv::Point closest = segments[j].FindClosestPoint(groundTruth[i], d);
				if(d < minDist)
				{
					minJ = j;
					minDist = d;
				}
			}

			if(minDist < BenchmarkParams.MinError)
			{
				TP++;
			}
			error += minDist;
		}

		//Calculate the True positive
		TP /= (double)groundTruth.size();
		error /= (double)groundTruth.size();

		return TP;
	}

	double CalcFalsePositives(const std::vector<DuneSegment> &segments,
									const std::vector<cv::Point> &groundTruth)
	{
		double FP = 0;
		//Calculate the false positives
		//For each segment
		int totalPoints = 0;
		for(size_t i = 0; i < segments.size(); ++i)
		{
			//For each point in the segment;
			for(size_t j = 0; j < segments[i].Data.size(); ++j)
			{
				double minDist = DBL_MAX;
				//Find the closest ground truth to that point
				for(size_t k = 0; k < groundTruth.size(); ++k)
				{
					double d = std::sqrt(((segments[i].Data[j].Position.x-groundTruth[k].x) *
								(segments[i].Data[j].Position.x-groundTruth[k].x)) +
								((segments[i].Data[j].Position.y-groundTruth[k].y) *
								(segments[i].Data[j].Position.y-groundTruth[k].y)));
					if(d < minDist)
						minDist = d;
				}

				//If there's no ground truth near that segment point, it's a FP
				if(minDist > BenchmarkParams.MinError)
				{
					FP++;
				}

				totalPoints++;
			}
		}

		FP /= (double)totalPoints;

		return FP;
	}

};

}

#endif