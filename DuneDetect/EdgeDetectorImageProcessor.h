#ifndef _EDGE_DETECTOR_IMAGE_PROCESSOR_H_
#define _EDGE_DETECTOR_IMAGE_PROCESSOR_H_

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class EdgeDetectorProcParams
	{
	public:
		EdgeDetectorProcParams()
		{
			K = 5;
			NumScales = 20;
			MinMatch = 6;
			DistanceThreshold = 1.0;
		}
		~EdgeDetectorProcParams() {}
		EdgeDetectorProcParams(const EdgeDetectorProcParams &cpy)
		{
			K = cpy.K;
			NumScales = cpy.NumScales;
			MinMatch = cpy.MinMatch;
			DistanceThreshold = cpy.DistanceThreshold;
		}
		EdgeDetectorProcParams(int pK, int pNumScales, int pMinMatch, double pDistThresh)
		{
			K = pK;
			NumScales = pNumScales;
			MinMatch = pMinMatch;
			DistanceThreshold = pDistThresh;
		}

		int K;
		int NumScales;
		int MinMatch;
		double DistanceThreshold;
	};

	class EdgeDetectorImageProcessor : public BasedEdgeImageProcessor
	{
	public:
		EdgeDetectorImageProcessor();
		~EdgeDetectorImageProcessor();
		EdgeDetectorImageProcessor(const EdgeDetectorImageProcessor &cpy);
		EdgeDetectorImageProcessor(const EdgeDetectorProcParams &params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

	private:

		void GetMultiScaleCanny(const cv::Mat &img, cv::Mat &canny);
		void GetCannyImage(const cv::Mat &img, cv::Mat &canny);

		double CalcAverageGradient(const cv::Mat &img, int k, double &stdev, double &orientation);
		double CalcAverageGradientLaplacian(const cv::Mat &img, int k, double &stdev);

		void ComputeMaximallyStableEdges(const cv::Mat &img, cv::Mat &stable, double minQ, double maxQ, int numIterations, int t);
		void GetNormalizedCanny(const cv::Mat &img, cv::Mat &canny, double minT, double maxT, int size);

		EdgeDetectorProcParams parameters;

		//////////////////////////////////////////////////////////////////////////////////
		//Old crap that probably won't work but you never know
		//////////////////////////////////////////////////////////////////////////////////
		void RemoveLowerIntensity(const cv::Mat &input, cv::Mat &output, double q);
		void ImageNegative(cv::Mat &img);

		std::vector<cv::Mat> edgeMap;
		cv::Mat distanceKernel;
		void InitDistanceKernel();
		cv::Mat FilterEdges();
		bool SearchScales(const cv::Point &inPt, cv::Point &outPt);
		double GetNNPixelDistanceAt(const cv::Point &at, int edgeMapIndex, cv::Point &nn);

		//////////////////////////////////////////////////////////////////////////////////
		//Old crap that probably won't work but you never know
		//////////////////////////////////////////////////////////////////////////////////
	};



}

#endif