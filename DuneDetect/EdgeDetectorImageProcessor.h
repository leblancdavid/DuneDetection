#ifndef _EDGE_DETECTOR_IMAGE_PROCESSOR_H_
#define _EDGE_DETECTOR_IMAGE_PROCESSOR_H_

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class EdgeDetectorProcParams : public BaseImageProcessParameters
	{
	public:
		EdgeDetectorProcParams()
		{
			K = 7;
			EdgeThreshold = 3.1416 / 2.0;
			DominantOrientationBins = 16;

			//Not used
			NumScales = 12;
			MinMatch = 6;
			DistanceThreshold = 1.0;
		}
		~EdgeDetectorProcParams() {}
		EdgeDetectorProcParams(const EdgeDetectorProcParams &cpy)
		{
			K = cpy.K;
			EdgeThreshold = cpy.EdgeThreshold;
			DominantOrientationBins = cpy.DominantOrientationBins;

			//Not used
			NumScales = 12;
			MinMatch = 6;
			DistanceThreshold = 1.0;
		}
		EdgeDetectorProcParams(int pK, double edgeThresh, int dominantOrientationBins)
		{
			K = pK;
			EdgeThreshold = edgeThresh;
			DominantOrientationBins = dominantOrientationBins;
			
			//Not used
			NumScales = 12;
			MinMatch = 6;
			DistanceThreshold = 1.0;
		}

		int K;
		double EdgeThreshold;
		int DominantOrientationBins;
		///Not currently in use.
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
		EdgeDetectorImageProcessor(EdgeDetectorProcParams *params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

		void SetParameters(BaseImageProcessParameters *params)
		{
			parameters = static_cast<EdgeDetectorProcParams*>(params);
		}

	private:

		void GetMultiScaleCanny(const cv::Mat &img, cv::Mat &canny);
		void GetCannyImage(const cv::Mat &img, cv::Mat &canny);

		void GetSobelImage(const cv::Mat &img, cv::Mat &sobel);

		void ComputeMaximallyStableEdges(const cv::Mat &img, cv::Mat &stable, double minQ, double maxQ, int numIterations, int t);
		void GetNormalizedCanny(const cv::Mat &img, cv::Mat &canny, double minT, double maxT, int size);

		EdgeDetectorProcParams *parameters;

		double FindOptimalScale(const cv::Mat &img);

		void ThresholdByEdgeDirection(const cv::Mat &img, cv::Mat &output, double direction);

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