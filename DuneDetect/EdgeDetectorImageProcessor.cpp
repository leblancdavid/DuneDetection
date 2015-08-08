#include "EdgeDetectorImageProcessor.h"
#include "GenericImageProcessing.h"


namespace dune
{
	EdgeDetectorImageProcessor::EdgeDetectorImageProcessor()
	{
		parameters = new EdgeDetectorProcParams();
		InitDistanceKernel();
	}

	EdgeDetectorImageProcessor::~EdgeDetectorImageProcessor()
	{
		delete parameters;
	}

	EdgeDetectorImageProcessor::EdgeDetectorImageProcessor(const EdgeDetectorImageProcessor &cpy)
	{
		parameters = cpy.parameters;
		InitDistanceKernel();
	}

	EdgeDetectorImageProcessor::EdgeDetectorImageProcessor(EdgeDetectorProcParams *params)
	{
		parameters = params;
		InitDistanceKernel();
	}

	void EdgeDetectorImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		int k = parameters->K;
		cv::Mat filtered, bilateral, threshold, canny;
		cv::medianBlur(inputImg, filtered, parameters->K);
		cv::bilateralFilter(filtered, bilateral, parameters->K, 20, 20);
		//cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);
		//cv::GaussianBlur(filtered, filtered, cv::Size(parameters.K, parameters.K), parameters.K / 5.0, parameters.K / 5.0);
		//The bilateral filter is used for computing the gradient for now, seems to work well.
		
		// Apparently the gaussian blurring is messing up the accuracy of the detection, so for now I will keep it commented out.
		
		cv::equalizeHist(bilateral, filtered);
		ComputeGradient(filtered, k);
		
		//GetCannyImage(filtered, outputImg);
		double orientation = FindDominantOrientation(DominantOrientationMethod::HOG, parameters->DominantOrientationBins);


		ThresholdByEdgeDirection(inputImg, outputImg, orientation);
		//GetSobelImage(bilateral, outputImg);
		//cv::imshow("outputImg", outputImg);
		//cv::waitKey(0);

		//imgproc::IntegralEdgeThreshold(bilateral, canny, 20, k);
		//cv::erode(canny, canny, cv::Mat(), cv::Point(-1, -1), 6);
		//cv::dilate(canny, canny, cv::Mat(), cv::Point(-1, -1), 4);

		//cv::bitwise_and(canny, outputImg, outputImg);
		//cv::imshow("Canny", canny);
		//cv::waitKey(0);
		//This is somewhat hacky, but this basically ensures that an edge line always has only 1 or 2 neighbors.
		//imgproc::ThinCannyEdges(canny, outputImg);
	}

	void EdgeDetectorImageProcessor::GetMultiScaleCanny(const cv::Mat &img, cv::Mat &canny)
	{
		cv::Mat resized, result;

		double scale = FindOptimalScale(img);
		cv::resize(img, resized, cv::Size(img.cols*scale, img.rows*scale));
		//cv::GaussianBlur(img, resized, cv::Size(scale, scale), scale / 5.0, scale / 5.0);
		
		ComputeGradient(resized, parameters->K);

		double stdev = BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];
		double average = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
		double q = 3.0;
		double cannyHighThreshold = average + q*stdev;
		cv::Canny(resized, result, cannyHighThreshold, cannyHighThreshold / 2.0, parameters->K);

		cv::resize(result, canny, cv::Size(img.cols, img.rows));
	}

	void EdgeDetectorImageProcessor::GetCannyImage(const cv::Mat &img, cv::Mat &canny)
	{
		double stdev = BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];
		double average = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
		double q = 2.5;
		double cannyHighThreshold = average + q*stdev;
		cv::Canny(img, canny, cannyHighThreshold, cannyHighThreshold / 2.0, parameters->K);
	}

	void EdgeDetectorImageProcessor::GetSobelImage(const cv::Mat &img, cv::Mat &sobel)
	{
		double stdev = BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];
		double average = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
		double q = 0.0;
		double threshold = average + q*stdev;
		cv::Mat dx, dy, mag;

		cv::Sobel(img, dx, CV_64F, 1, 0, parameters->K);
		cv::Sobel(img, dy, CV_64F, 0, 1, parameters->K);
		cv::magnitude(dx, dy, mag);

		sobel = cv::Mat(mag.rows, mag.cols, CV_8U);
		for (int x = 0; x < mag.cols; ++x)
		{
			for (int y = 0; y < mag.rows; ++y)
			{
				if (mag.at<double>(y, x) > threshold)
				{
					sobel.at<uchar>(y, x) = 255;
				}
				else
				{
					sobel.at<uchar>(y, x) = 0;
				}
			}
		}

	}

	void EdgeDetectorImageProcessor::ThresholdByEdgeDirection(const cv::Mat &img, cv::Mat &output, double direction)
	{
		output = img.clone();
		double thresh = parameters->EdgeThreshold;
		for (int x = 0; x < img.cols; ++x)
		{
			for (int y = 0; y < img.rows; ++y)
			{
				double d = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DIRECTION_INDEX];
				double low = fabs(d - direction);
				double high = fabs(d - (direction + 2.0*3.1416));
				if (low < thresh ||
					high < thresh)
				{
					output.at<uchar>(y, x) = 255;
				}
				else
				{
					output.at<uchar>(y, x) = 0;
				}
			}
		}

		//cv::imshow("threshold image", output);
		//cv::waitKey(0);
		//cv::imwrite("NormalizedDirectionImage.jpg", tempImage);
		//cv::imwrite("ThresholdNormalizedDirectionImage.jpg", output);
	}


	void EdgeDetectorImageProcessor::ComputeMaximallyStableEdges(const cv::Mat &img, cv::Mat &stable, double minQ, double maxQ, int numIterations, int t)
	{
		int k = 5;
		cv::Mat filtered, threshold, canny, cannySum = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
		cv::medianBlur(img, filtered, k);
		cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 4.7, (double)k / 4.7);
		double stdev = BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];
		double average = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];

		double itQ = (maxQ - minQ) / (double)(numIterations - 1);
		double cannyHighThreshold;
		double q = minQ;
		for (int i = 0; i < numIterations; ++i)
		{
			cannyHighThreshold = average + q*stdev;
			GetNormalizedCanny(filtered, canny, cannyHighThreshold / 2.0, cannyHighThreshold, k);
			//canny.mul(cv::Mat(), (double)255/numIterations);
			cannySum = cannySum + canny;
			q += itQ;
		}

		//cv::imshow("The Sum Image", cannySum);
		//cv::waitKey(0);

		stable = cv::Mat(cannySum.rows, cannySum.cols, CV_8UC1);
		for (int x = 0; x < stable.cols; ++x)
		{
			for (int y = 0; y < stable.rows; ++y)
			{
				uchar v = cannySum.at<uchar>(y, x);
				if (cannySum.at<uchar>(y, x) > t)
				{
					stable.at<uchar>(y, x) = 255;
				}
				else
				{
					stable.at<uchar>(y, x) = 0;
				}
			}
		}


	}

	void EdgeDetectorImageProcessor::GetNormalizedCanny(const cv::Mat &img, cv::Mat &canny, double minT, double maxT, int size)
	{
		cv::Canny(img, canny, maxT, minT, size);
		cv::divide(canny, cv::Scalar(255), canny);
		//canny = canny / cv::Scalar(255);
	}


	double EdgeDetectorImageProcessor::FindOptimalScale(const cv::Mat &img)
	{
		cv::Mat sobel_x, sobel_y;
		cv::Mat laplacian;
		cv::Mat resized, result;
		double maxScale = 0, maxR = 0;
		int maxIndex = 0;
		std::vector<double> scaleMagnitudes;
		std::vector<double> scaleStdDev;
		for (int i = 0; i < parameters->NumScales; ++i)
		{
			//double scale = 3.0 + (double)i*2.0;
			double scale = 1.0 - (double)i*0.05;
			//cv::GaussianBlur(img, resized, cv::Size(scale, scale), scale / 5.0, scale / 5.0);
			cv::resize(img, resized, cv::Size(img.cols*scale, img.rows*scale));
			cv::GaussianBlur(resized, resized, cv::Size(3, 3), 3.0 / 5.0, 3.0 / 5.0);
			ComputeGradient(resized, parameters->K);
			double stdev = BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];
			double average = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
			double q = 3.0;
			double cannyHighThreshold = average + q*stdev;
			cv::Canny(resized, result, cannyHighThreshold, cannyHighThreshold / 2.0, parameters->K);
			cv::imshow("Canny", result);
			cv::waitKey(0);
			//cv::Sobel(resized, sobel_x, CV_64F, 1, 0, parameters.K);
			//cv::Sobel(resized, sobel_y, CV_64F, 0, 1, parameters.K);
			//cv::Scalar u_x, u_y, sig_x, sig_y;
			//cv::meanStdDev(sobel_x, u_x, sig_x);
			//cv::meanStdDev(sobel_y, u_y, sig_y);

			//scaleMagnitudes.push_back(std::sqrt(u_x[0] * u_x[0] + u_y[0] * u_y[0])*scale);
			//scaleStdDev.push_back(std::sqrt(sig_x[0] * sig_x[0] + sig_y[0] * sig_y[0])*scale);


			cv::Laplacian(resized, laplacian, CV_64F, parameters->K * 2 + 1);
			cv::Scalar u, sig;
			cv::meanStdDev(laplacian, u, sig);

			scaleMagnitudes.push_back(u[0]);
			scaleStdDev.push_back(sig[0]);

			if (scaleStdDev.back() > maxR)
			{
				maxR = scaleStdDev.back();
				maxIndex = i;
				maxScale = scale;

			}
		}

		int peak = 0;
		for (size_t i = 1; i < scaleMagnitudes.size()-1; ++i)
		{
			if (scaleMagnitudes[i] > scaleMagnitudes[i - 1] &&
				scaleMagnitudes[i] > scaleMagnitudes[i + 1])
			{
				peak = i;
				break;
			}
		}


		return 1.0 - (double)peak * 0.05;;
	}











	//////////////////////////////////////////////////////////////////////////////////
	//Old crap that probably won't work but you never know
	//////////////////////////////////////////////////////////////////////////////////

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
			if (dist < parameters->DistanceThreshold)
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

		if (avgCount > parameters->MinMatch)
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
		double minDist = parameters->DistanceThreshold * 2.0;
		for (int x = at.x - distanceKernel.cols / 2, i = 0; x < at.x + distanceKernel.cols / 2; ++x, ++i)
		{
			if (x < 0 || x >= edgeMap[edgeMapIndex].cols)
				continue;

			for (int y = at.y - distanceKernel.rows / 2, j = 0; y < at.y + distanceKernel.rows / 2; ++y, ++j)
			{
				if (y < 0 || y >= edgeMap[edgeMapIndex].rows)
					continue;

				if (edgeMap[edgeMapIndex].at<uchar>(y, x)
					&& distanceKernel.at<double>(j, i) < minDist)
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
		int size = parameters->DistanceThreshold * 2.0;
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


}