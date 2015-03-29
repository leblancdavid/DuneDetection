#ifndef _ROTATED_KERNEL_EDGE_DETECTOR_H_
#define _ROTATED_KERNEL_EDGE_DETECTOR_H_

#include "BaseEdgeImageProcessor.h"

namespace dune
{
	class RotatedKernelEdgeDetectorParameters
	{
	public:
		RotatedKernelEdgeDetectorParameters()
		{
			K = 15;
			NumDirections = 12;
		}
		~RotatedKernelEdgeDetectorParameters() {}
		RotatedKernelEdgeDetectorParameters(const RotatedKernelEdgeDetectorParameters &cpy)
		{
			K = cpy.K;
			NumDirections = cpy.NumDirections;
		}
		RotatedKernelEdgeDetectorParameters(int pK, int pNumDirections)
		{
			K = pK;
			NumDirections = pNumDirections;
		}

		int K;
		int NumDirections;
	};

	class RotatedKernelEdgeDetector : public BasedEdgeImageProcessor
	{
	public:
		RotatedKernelEdgeDetector();
		~RotatedKernelEdgeDetector();
		RotatedKernelEdgeDetector(const RotatedKernelEdgeDetector &cpy);
		RotatedKernelEdgeDetector(const RotatedKernelEdgeDetectorParameters &params);

		void Process(const cv::Mat &inputImg, cv::Mat &outputImg);

	private:

		RotatedKernelEdgeDetectorParameters Parameters;
		std::vector<cv::Mat> Kernels;

		void InitKernels();
		void ConvolveImage(const cv::Mat &input, cv::Mat &output);
	};



}

#endif