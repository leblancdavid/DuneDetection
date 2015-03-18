#include "EdgeDetectorImageProcessor.h"

namespace dune
{
	EdgeDetectorImageProcessor::EdgeDetectorImageProcessor()
	{
		InitDistanceKernel();
	}

	EdgeDetectorImageProcessor::~EdgeDetectorImageProcessor()
	{

	}

	EdgeDetectorImageProcessor::EdgeDetectorImageProcessor(const EdgeDetectorImageProcessor &cpy)
	{
		parameters = cpy.parameters;
		InitDistanceKernel();
	}

	EdgeDetectorImageProcessor::EdgeDetectorImageProcessor(const EdgeDetectorProcParams &params)
	{
		parameters = params;
		InitDistanceKernel();
	}

	void EdgeDetectorImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		int k = 5;
		cv::Mat filtered, threshold, canny;
		//cv::equalizeHist(inputImg, filtered);
		cv::medianBlur(inputImg, filtered, k);
		cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 4.7, (double)k / 4.7);

		//ImageNegative(filtered);
		//RemoveLowerIntensity(filtered, threshold, -1.0);

		//cv::imshow("Threshold Image", threshold);
		//cv::waitKey(0);

		double stdev, orientation, average = CalcAverageGradient(filtered, k, stdev, orientation);

		/*cv::Mat orientationImg;
		cv::cvtColor(filtered, orientationImg, CV_GRAY2BGR);
		cv::Point center(orientationImg.cols / 2.0, orientationImg.rows / 2.0);
		cv::Point endPt(100.0*std::cos(orientation) + center.x, 100.0*std::sin(orientation) + center.y);
		cv::line(orientationImg, center, endPt, cv::Scalar(0, 0, 255), 2);
		cv::imshow("Average Orientation", orientationImg);
		cv::waitKey(0);*/

		double q = 2.0;
		double c_High = average + q*stdev;
		double c_Low = c_High / 2.0;
		edgeMap.resize(parameters.NumScales+1);
		cv::Canny(filtered, canny, c_High, c_Low, k);
		cv::imshow("Canny Image", canny);
		cv::waitKey(0);
		//cv::imwrite("EdgeMap.jpg", edgeMap[0]);
		//cv::imshow("Results", edgeMap[0]);
		//cv::waitKey(0);
		//cv::Mat blurred;
		//cv::Size kernelSize = cv::Size(3, 3);
		//double sigma = 1.2;
		////blurred = medianFiltered.clone();
		//cv::Size size = filtered.size();
		//cv::Mat resizedImg;
		//for (int i = 1; i <= parameters.NumScales; ++i)
		//{
		//	//cv::medianBlur(medianFiltered, blurred, kernelSize.width);
		//	//cv::GaussianBlur(medianFiltered, blurred, kernelSize, sigma, sigma);
		//	size.width *= 0.9;
		//	size.height *= 0.9;

		//	cv::resize(filtered, resizedImg, size);

		//	average = CalcAverageGradient(resizedImg, k, stdev, orientation);
		//	c_High = average + q*stdev;
		//	c_Low = c_High / 2.0;

		//	cv::Canny(resizedImg, edgeMap[i], c_High, c_Low, k);

		//	/*kernelSize.width += 2;
		//	kernelSize.height += 2;
		//	sigma += 0.25;*/

		//	cv::imshow("Results", edgeMap[i]);
		//	cv::waitKey(0);
		//}

		//outputImg = FilterEdges();
		//cv::imshow("Filtered", outputImg);
		//cv::waitKey(0);

		outputImg = canny.clone();
	}

	void EdgeDetectorImageProcessor::ImageNegative(cv::Mat &img)
	{
		for (int x = 0; x < img.cols; ++x)
		{
			for (int y = 0; y < img.rows; ++y)
			{
				img.at<uchar>(y, x) = 255 - img.at<uchar>(y, x);
			}
		}
	}

	void EdgeDetectorImageProcessor::RemoveLowerIntensity(const cv::Mat &input, cv::Mat &output, double q)
	{
		double avgIntensity = 0;
		double stdDevIntensity = 0;
		for (int x = 0; x < input.cols; ++x)
		{
			for (int y = 0; y < input.rows; ++y)
			{
				avgIntensity += (double)input.at<uchar>(y, x) / 255.0;
			}
		}

		avgIntensity /= (double)(input.cols*input.rows) / 255.0;

		for (int x = 0; x < input.cols; ++x)
		{
			for (int y = 0; y < input.rows; ++y)
			{
				stdDevIntensity += fabs(((double)input.at<uchar>(y, x) - avgIntensity)) / 255.0;
			}
		}

		stdDevIntensity /= (double)(input.cols*input.rows) / 255.0;

		double scaler = 30.0 / stdDevIntensity;
		output = cv::Mat::zeros(cv::Size(input.cols, input.rows), CV_8UC1);
		for (int x = 0; x < output.cols; ++x)
		{
			for (int y = 0; y < output.rows; ++y)
			{
				double val = (double)(input.at<uchar>(y, x) - avgIntensity - (q*stdDevIntensity))*scaler;
				if (val < 0)
					output.at<uchar>(y, x) = 0;
				else
					output.at<uchar>(y, x) = (uchar)val;
			}
		}
	}

	double EdgeDetectorImageProcessor::CalcAverageGradient(const cv::Mat &img, int k, double &stdev, double &orientation)
	{
		cv::Mat sobel_x, sobel_y;
		cv::Sobel(img, sobel_x, CV_64F, 1, 0, k);
		cv::Sobel(img, sobel_y, CV_64F, 0, 1, k);

		double average = 0;
		double dx = 0, dy = 0;
		stdev = 0;
		for (int x = 0; x < sobel_x.cols; ++x)
		{
			for (int y = 0; y < sobel_x.rows; ++y)
			{
				dx += sobel_x.at<double>(y, x) / 1000.0;
				dy += sobel_y.at<double>(y, x) / 1000.0;
				double g = std::sqrt(sobel_x.at<double>(y, x)*sobel_x.at<double>(y, x) +
					sobel_y.at<double>(y, x)*sobel_y.at<double>(y, x))/1000.0;
				average += g;

			}
		}

		average /= sobel_x.rows*sobel_x.cols;
		average *= 1000.0;

		orientation = std::atan2(dy, dx);

		for (int x = 0; x < sobel_x.cols; ++x)
		{
			for (int y = 0; y < sobel_x.rows; ++y)
			{
				double g = std::sqrt(sobel_x.at<double>(y, x)*sobel_x.at<double>(y, x) +
					sobel_y.at<double>(y, x)*sobel_y.at<double>(y, x));
				stdev += (g-average)*(g-average) / 1000.0;
			}
		}

		stdev /= sobel_x.rows*sobel_x.cols;
		stdev *= 1000.0;
		stdev = std::sqrt(stdev);

		return average;
	}

	cv::Mat EdgeDetectorImageProcessor::FilterEdges()
	{
		cv::Mat filteredEdges = cv::Mat::zeros(edgeMap[0].rows, edgeMap[0].cols, CV_8U);

		for (int x = 0; x < edgeMap[0].cols; ++x)
		{
			for (int y = 0; y < edgeMap[0].rows; ++y)
			{
				if (edgeMap[0].at<uchar>(y, x))
				{
					cv::Point newPoint;
					if (SearchScales(cv::Point(x, y), newPoint))
					{
						filteredEdges.at<uchar>(cv::Point(x, y)) = 255;
					}

				}
			}
		}

		return filteredEdges;
	}

	bool EdgeDetectorImageProcessor::SearchScales(const cv::Point &inPt, cv::Point &outPt)
	{
		outPt = inPt;
		double avgCount = 1.0;
		double avgDist = 0.0;

		for (size_t i = 1; i < edgeMap.size(); ++i)
		{
			cv::Point nn;
			double dist = GetNNPixelDistanceAt(inPt, i, nn);
			if (dist < parameters.DistanceThreshold)
			{
				avgDist += dist;
				outPt.x += nn.x;
				outPt.y += nn.y;
				avgCount++;
			}
		}

		avgDist /= avgCount;
		outPt.x /= avgCount;
		outPt.y /= avgCount;

		if (avgCount > parameters.MinMatch)
		{
			return true;
		}
		else
		{

			return false;
		}
	}

	double EdgeDetectorImageProcessor::GetNNPixelDistanceAt(const cv::Point &at, int edgeMapIndex, cv::Point &nn)
	{
		double minDist = parameters.DistanceThreshold * 2.0;
		for (int x = at.x - distanceKernel.cols/2, i = 0; x < at.x + distanceKernel.cols/2; ++x, ++i)
		{
			if (x < 0 || x >= edgeMap[edgeMapIndex].cols)
				continue;
			
			for (int y = at.y - distanceKernel.rows / 2, j = 0; y < at.y + distanceKernel.rows / 2; ++y, ++j)
			{
				if (y < 0 || y >= edgeMap[edgeMapIndex].rows)
					continue;

				if (edgeMap[edgeMapIndex].at<uchar>(y, x)
					&& distanceKernel.at<double>(j,i) < minDist)
				{
					minDist = distanceKernel.at<double>(j, i);
					nn.x = x;
					nn.y = y;
				}
			}
		}

		return minDist;
	}


	void EdgeDetectorImageProcessor::InitDistanceKernel()
	{
		int size = parameters.DistanceThreshold * 2.0;
		//Make sure the kernel size is odd
		if (size % 2 == 0)
		{
			size++;
		}
		int half = size / 2;
		distanceKernel = cv::Mat(size, size, CV_64F);
		
		for (int x = 0; x < size; ++x)
		{
			for (int y = 0; y < size; ++y)
			{
				distanceKernel.at<double>(y, x) = std::sqrt((double)((x - half)*(x - half) + (y - half)*(y - half)));
			}
		}
	}

}