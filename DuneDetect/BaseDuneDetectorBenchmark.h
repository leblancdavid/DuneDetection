#ifndef _DUNE_DETECTOR_BENCHMARK_TEST_H_
#define _DUNE_DETECTOR_BENCHMARK_TEST_H_

#include "BaseDuneDetector.h"
#include "DuneMetricCalculator.h"
#include "Draw.h"

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

	virtual ~BenchmarkResults() {}

	double GetPrecision()
	{
		if (fabs(TP + FP) <= 0.01)
			return 0.0;

		return TP / (TP + FP);
	}
	
	double GetRecall()
	{
		if (fabs(TP + FN) <= 0.01)
			return 0.0;

		return TP / (TP + FN);
	}


	double TP;
	double FP;
	double TN;
	double FN;
	double Error;
	double AngularError;
	double SpacingError;
	double NormSpacingError;

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
	BaseDuneDetectorBenchmark(BaseDuneDetector* detector, const BenchmarkTestParameters &params)
	{
		Detector = detector;
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

		cv::Mat colorImg = cv::Mat::zeros(groundTruthImg.rows, groundTruthImg.cols, CV_8UC3);
		cv::cvtColor(testImg, colorImg, CV_GRAY2BGR);

		BenchmarkResults results;

		results = GetBenchmarkResults(segments, groundTruth, colorImg);

		cv::imshow("Detected Points", colorImg);
		//cv::imwrite("Method4Results.jpg", colorImg);
		cv::waitKey(33);
		return results;
	}

	BenchmarkResults GetResults(const cv::Mat & testImg,
		const cv::Mat &groundTruthImg,
		const std::vector<DuneSegment> &segments,
		const std::string &resultFileName="")
	{
		dune::imgproc::DuneMetricCalculator metrics;
		
		std::vector<cv::Point> groundTruth = GetGroundTruthPoints(groundTruthImg);

		BenchmarkResults results;
		cv::Mat colorImg = cv::Mat::zeros(groundTruthImg.rows, groundTruthImg.cols, CV_8UC3);
		cv::cvtColor(testImg, colorImg, CV_GRAY2BGR);

		cv::Mat segmentMap = cv::Mat::zeros(groundTruthImg.rows, groundTruthImg.cols, CV_8U);
		for (size_t i = 0; i < segments.size(); ++i)
		{
			for (size_t j = 0; j < segments[i].GetLength(); ++j)
			{
				segmentMap.at<uchar>(segments[i][j].Position) = 255;
			}
		}

		
		results = GetBenchmarkResults(segmentMap, groundTruthImg, colorImg);

		dune::imgproc::DuneMetrics groundTruthMetrics = metrics.CalculateMetrics(groundTruthImg);
		dune::imgproc::DuneMetrics detectionMetrics = metrics.CalculateMetrics(segmentMap);
		results.AngularError = std::fabs(groundTruthMetrics.AverageOrientation - detectionMetrics.AverageOrientation);
		results.SpacingError = std::fabs(groundTruthMetrics.AverageDuneSpacing - detectionMetrics.AverageDuneSpacing);
		results.NormSpacingError = results.SpacingError / groundTruthMetrics.AverageDuneSpacing;

		if (resultFileName != "")
			cv::imwrite(resultFileName, colorImg);
		
		return results;
	}

protected:
	BaseDuneDetector *Detector;
	
	BenchmarkResults GetBenchmarkResults(const cv::Mat &segmentMap, const cv::Mat &groundTruth, cv::Mat &plot)
	{
		BenchmarkResults results;
		CalculateTP_FN(segmentMap, groundTruth, plot, results);
		CalculateFP(segmentMap, groundTruth, plot, results);

		return results;
	}

	void CalculateTP_FN(const cv::Mat &segmentMap, const cv::Mat &groundTruth, cv::Mat &plot, BenchmarkResults &results)
	{
		results.FN = 0;
		results.TP = 0;

		for (int i = 0; i < groundTruth.rows; ++i)
		{
			for (int j = 0; j < groundTruth.cols; ++j)
			{
				if (groundTruth.at<uchar>(i, j))
				{
					if (ContainsDetection(segmentMap, cv::Point(j, i)))
					{
						results.TP++;
						//detected ground truth will be blue
						cv::circle(plot, cv::Point(j,i), 1, cv::Scalar(255, 0, 0), 2);
					}
					else
					{
						results.FN++;
						//false negatives are yellow
						cv::circle(plot, cv::Point(j, i), 1, cv::Scalar(0, 255, 255), 2);
					}
				}
			}
		}
	}

	void CalculateFP(const cv::Mat &segmentMap, const cv::Mat &groundTruth, cv::Mat &plot, BenchmarkResults &results)
	{
		results.FP = 0;

		for (int i = 0; i < segmentMap.rows; ++i)
		{
			for (int j = 0; j < segmentMap.cols; ++j)
			{
				if (segmentMap.at<uchar>(i, j))
				{
					if (ContainsDetection(groundTruth, cv::Point(j, i)))
					{
						//correctly detected points will be green
						cv::circle(plot, cv::Point(j, i), 1, cv::Scalar(0, 255, 0), 2);
					}
					else
					{
						results.FP++;
						//false positives are going to be red
						cv::circle(plot, cv::Point(j, i), 1, cv::Scalar(0, 0, 255), 2);
					}
				}
			}
		}
	}

	bool ContainsDetection(const cv::Mat &map, const cv::Point &anchor)
	{
		int i0 = anchor.y - BenchmarkParams.MinError;
		int i1 = anchor.y + BenchmarkParams.MinError;
		int j0 = anchor.x - BenchmarkParams.MinError;
		int j1 = anchor.x + BenchmarkParams.MinError;
		if (i0 < 0)
			i0 = 0;
		if (i1 >= map.rows)
			i1 = map.rows - 1;
		if (j0 < 0)
			j0 = 0;
		if (j1 >= map.cols)
			j1 = map.cols - 1;

		for (int i = i0; i <= i1; ++i)
		{
			for (int j = j0; j <= j1; ++j)
			{
				if (map.at<uchar>(i, j) > 0)
				{
					return true;
				}
			}
		}
		
		return false;
	}

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
		const std::vector<cv::Point> &groundTruth, cv::Mat &plot)
	{
		BenchmarkResults results;

		results.TP = CalcTruePositives(segments, groundTruth, results.FN, results.Error);
		results.FP = CalcFalsePositives(segments, groundTruth, plot);
		//CalculateResults(segments, groundTruth, results.TP, results.FP);
		
		//std::cout << results.TP << std::endl;

		return results;
	}
	
	void CalculateResults(const std::vector<DuneSegment> &segments,
		const std::vector<cv::Point> &groundTruth,
		double &TP, double &FP)
	{
		cv::Mat resultsImg = cv::Mat::zeros(800, 1200, CV_8UC3) ;

		TP = 0;
		std::vector<std::vector<bool>> matchFound(segments.size());
		double totalData = 0;
		for (size_t i = 0; i < matchFound.size(); ++i)
		{
			std::vector<DuneSegmentData> segData = segments[i].GetSegmentData();
			for (size_t j = 0; j < segData.size(); ++j)
			{
				matchFound[i].push_back(false);
				totalData++;

				resultsImg.at<cv::Vec3b>(segData[j].Position) = cv::Vec3b(0, 0, 255);
			}
		}

		for (size_t i = 0; i < groundTruth.size(); ++i)
		{
			resultsImg.at<cv::Vec3b>(groundTruth[i]) = cv::Vec3b(255, 0, 0);
		}

		//Compute the true positives;
		for (size_t i = 0; i < groundTruth.size(); ++i)
		{
			int minJ, minK;
			double minDist = DBL_MAX;
			bool found = false;
			for (size_t j = 0; j < segments.size(); ++j)
			{
				std::vector<DuneSegmentData> segData = segments[j].GetSegmentData();
				for (size_t k = 0; k < segData.size(); ++k)
				{
					if (matchFound[j][k])
						continue;

					double d = segData[k].DistanceFrom(groundTruth[i]);
					if (d < minDist)
					{
						minJ = j;
						minK = k;
						minDist = d;
					}
				}

				
			}

			if (minDist <= BenchmarkParams.MinError)
			{
				matchFound[minJ][minK] = true;
				TP++;

				resultsImg.at<cv::Vec3b>(groundTruth[i]) = cv::Vec3b(0, 255, 0);
				std::vector<DuneSegmentData> segData = segments[minJ].GetSegmentData();
				resultsImg.at<cv::Vec3b>(segData[minK].Position) = cv::Vec3b(0, 255, 0);
			}

			cv::imshow("Results Progression", resultsImg);
			cv::waitKey(33);
			//error += minDist;
		}

		//Calculate the True positive
		FP = TP / totalData;
		TP /= (double)groundTruth.size();
		//error /= (double)groundTruth.size();
	}

	double CalcTruePositives(const std::vector<DuneSegment> &segments,
		const std::vector<cv::Point> &groundTruth,
		double &FN,
		double &error)
	{
		double TP = 0;
		FN = 0;
		error = 0;
		//Compute the true positives;
		for(size_t i = 0; i < groundTruth.size(); ++i)
		{
			int minJ;
			double minDist = DBL_MAX;
			bool found = false;
			for (size_t j = 0; j < segments.size(); ++j)
			{
				double d;
				cv::Point closest = segments[j].FindClosestPoint(groundTruth[i], d);
				if (d < minDist)
				{
					minJ = j;
					minDist = d;
				}
			}

			//std::cout << minDist << std::endl;
			if(minDist <= BenchmarkParams.MinError)
			{
				TP++;
			}
			else
			{
				FN++;
			}
			error += minDist;
		}

		//Calculate the True positive
		TP /= (double)groundTruth.size();
		//error /= (double)groundTruth.size();
		//std::cout << TP << std::endl;
		return TP;
	}

	double CalcFalsePositives(const std::vector<DuneSegment> &segments,
									const std::vector<cv::Point> &groundTruth,
									cv::Mat &plot)
	{
		double FP = 0;
		//Calculate the false positives
		//For each segment
		int totalPoints = 0;
		for(size_t i = 0; i < segments.size(); ++i)
		{
			//For each point in the segment;
			std::vector<DuneSegmentData> segData = segments[i].GetSegmentData();
			for (size_t j = 0; j < segData.size(); ++j)
			{
				double minDist = DBL_MAX;
				//Find the closest ground truth to that point
				for(size_t k = 0; k < groundTruth.size(); ++k)
				{
					double d = std::sqrt(((segData[j].Position.x - groundTruth[k].x) *
						(segData[j].Position.x - groundTruth[k].x)) +
						((segData[j].Position.y - groundTruth[k].y) *
						(segData[j].Position.y - groundTruth[k].y)));
					if(d < minDist)
						minDist = d;
				}

				//If there's no ground truth near that segment point, it's a FP
				if(minDist > BenchmarkParams.MinError)
				{
					FP++;
					cv::circle(plot, segData[j].Position, 1, cv::Scalar(0, 0, 255), 2);
					//plot.at<cv::Vec3b>(segData[j].Position) = cv::Vec3b(0, 0, 255);
				}
				else
				{
					cv::circle(plot, segData[j].Position, 1, cv::Scalar(0, 255, 0), 2);
					//plot.at<cv::Vec3b>(segData[j].Position) = cv::Vec3b(0, 255, 0);
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