#ifndef _EDGE_DETECTOR_IMAGE_PROCESSOR_H_
#define _EDGE_DETECTOR_IMAGE_PROCESSOR_H_

#include "BaseImageProcessor.h"

namespace dune
{
	class EdgeDetectorProcParams
	{
	public:
		EdgeDetectorProcParams()
		{
			NumScales = 20;
			MinMatch = 6;
			DistanceThreshold = 1.0;
		}
		~EdgeDetectorProcParams() {}
		EdgeDetectorProcParams(const EdgeDetectorProcParams &cpy)
		{
			NumScales = cpy.NumScales;
			MinMatch = cpy.MinMatch;
			DistanceThreshold = cpy.DistanceThreshold;
		}
		EdgeDetectorProcParams(int pNumScales, int pMinMatch, double pDistThresh)
		{
			NumScales = pNumScales;
			MinMatch = pMinMatch;
			DistanceThreshold = pDistThresh;
		}

		int NumScales;
		int MinMatch;
		double DistanceThreshold;
	};

	class EdgeDetectorImageProcessor : public BaseImageProcessor
	{
	public:
		EdgeDetectorImageProcessor();
		~EdgeDetectorImageProcessor();
		EdgeDetectorImageProcessor(const EdgeDetectorImageProcessor &cpy);
		EdgeDetectorImageProcessor(const EdgeDetectorProcParams &params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

	private:

		void RemoveLowerIntensity(const cv::Mat &input, cv::Mat &output, double q);
		void ImageNegative(cv::Mat &img);
		//void RemoveUpperIntensity(const cv::Mat &input, cv::Mat &output, double q);
		double CalcAverageGradient(const cv::Mat &img, int k, double &stdev, double &orientation);

		cv::Mat FilterEdges();
		bool SearchScales(const cv::Point &inPt, cv::Point &outPt);
		double GetNNPixelDistanceAt(const cv::Point &at, int edgeMapIndex, cv::Point &nn);

		EdgeDetectorProcParams parameters;

		std::vector<cv::Mat> edgeMap;
		cv::Mat distanceKernel;

		void InitDistanceKernel();
	};



}

#endif