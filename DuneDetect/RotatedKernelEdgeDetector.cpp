#include "RotatedKernelEdgeDetector.h"

namespace dune
{

RotatedKernelEdgeDetector::RotatedKernelEdgeDetector()
{
	InitKernels();
}

RotatedKernelEdgeDetector::~RotatedKernelEdgeDetector()
{

}

RotatedKernelEdgeDetector::RotatedKernelEdgeDetector(const RotatedKernelEdgeDetector &cpy)
{
	Parameters = cpy.Parameters;
	InitKernels();
}

RotatedKernelEdgeDetector::RotatedKernelEdgeDetector(const RotatedKernelEdgeDetectorParameters &params)
{
	Parameters = params;
	InitKernels();
}

void RotatedKernelEdgeDetector::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
{
	double k = 5;
	cv::Mat filtered;
	cv::medianBlur(inputImg, filtered, k);
	cv::GaussianBlur(filtered, filtered, cv::Size(k,k), (double)k / 4.7, (double)k / 4.7);

	ConvolveImage(filtered, outputImg);
}

void RotatedKernelEdgeDetector::ConvolveImage(const cv::Mat &input, cv::Mat &output)
{
	cv::Mat sobelTestX, sobelTestY, sobel(input.rows, input.cols, CV_64F);
	cv::Sobel(input, sobelTestX, CV_64F, 1, 0, Parameters.K);
	cv::Sobel(input, sobelTestY, CV_64F, 0, 1, Parameters.K);

	cv::Mat edges(input.rows, input.cols, CV_64F);
	output = cv::Mat(input.rows, input.cols, CV_8UC1);

	std::vector<cv::Mat> filteredImages(Kernels.size());
	for (size_t i = 0; i < Kernels.size(); ++i)
	{
		double t = Kernels[i].at<double>(0, 0);
		cv::filter2D(input, filteredImages[i], CV_64F, Kernels[i]);
	}

	for (int x = 0; x < input.cols; ++x)
	{
		for (int y = 0; y < input.rows; ++y)
		{
			std::vector<double> vals;
			double max = 0.0;
			int maxIndex = 0;
			for (size_t i = 0; i < filteredImages.size(); ++i)
			{
				double v = filteredImages[i].at<double>(y, x);
				vals.push_back(v);
				if (std::fabs(v) > max)
				{
					max = std::fabs(v);
					maxIndex = i;
				}
			}

			double dx = sobelTestX.at<double>(y, x);
			double dy = sobelTestY.at<double>(y, x);

			int test = 0;

			edges.at<double>(y, x) = max;
			sobel.at<double>(y, x) = std::sqrt(dx*dx + dy*dy);
		}
	}
	
	cv::normalize(edges, output, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::threshold(output, output, 66, 255, CV_THRESH_BINARY);

	cv::Mat sobelB;
	cv::normalize(sobel, sobelB, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::threshold(sobelB, sobelB, 66, 255, CV_THRESH_BINARY);

	cv::imshow("RotatedKernel", output);
	cv::imshow("Sobel", sobelB);
	cv::waitKey(0);
}

void RotatedKernelEdgeDetector::InitKernels()
{
	//First figure out how big we need to make the masks
	double r = (double)Parameters.K / 2.0;
	r = 2.0 * std::sqrt(r*r * 2.0);
	int size = std::ceil(r);
	//make sure the mask has odd size
	if (size % 2 == 0)
		size++;

	cv::Mat sobel, temp;
	cv::getDerivKernels(sobel, temp, 1, 1, Parameters.K, false, CV_64F);

	cv::Mat kernel, rKernel;
	kernel = cv::Mat::zeros(size, size, CV_64F);
	int startIdx = (size - Parameters.K) / 2;
	for (int x = 0; x < Parameters.K; ++x)
	{
		for (int y = 0; y < Parameters.K; ++y)
		{
			double s = sobel.at<double>(y, 0);
			kernel.at<double>(y + startIdx, x + startIdx) = sobel.at<double>(y, 0);
		}
	}

	Kernels.clear();
	Kernels.push_back(kernel.clone());
	double angleIt = (3.14159) / (double)(Parameters.NumDirections+1);
	for (int i = 1; i < Parameters.NumDirections; ++i)
	{
		double angle = ((double)i * angleIt) * 180.0 / 3.1416;
		cv::Mat rotMat = cv::getRotationMatrix2D(cv::Point2f((float)size / 2.0f, (float)size / 2.0f), angle, 1.0);
		cv::warpAffine(kernel, rKernel, rotMat, cv::Size(size, size));
		Kernels.push_back(rKernel.clone());
	}
	

}

}