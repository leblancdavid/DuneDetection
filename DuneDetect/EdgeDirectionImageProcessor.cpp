#include "EdgeDirectionImageProcessor.h"
#include "GenericImageProcessing.h"
#include "GaussianScalePyramid.h"

namespace dune
{
	EdgeDirectionImageProcessor::EdgeDirectionImageProcessor()
	{
		parameters = new EdgeDirectionProcParams();
	}

	EdgeDirectionImageProcessor::~EdgeDirectionImageProcessor()
	{
		delete parameters;
	}

	EdgeDirectionImageProcessor::EdgeDirectionImageProcessor(const EdgeDirectionImageProcessor &cpy)
	{
		parameters = cpy.parameters;
	}

	EdgeDirectionImageProcessor::EdgeDirectionImageProcessor(EdgeDirectionProcParams *params)
	{
		parameters = params;
	}

	void EdgeDirectionImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		int k = parameters->K;
		cv::Mat filtered, bilateral, threshold, canny;
		cv::medianBlur(inputImg, filtered, parameters->K);
		//cv::GaussianBlur(filtered, filtered, cv::Size(parameters->K, parameters->K), parameters->K / 5.0, parameters->K / 5.0);
		//cv::adaptiveBilateralFilter(filtered, bilateral, cv::Size(5, 5), 1.5);

		GaussianScalePyramid gsp;
		gsp.Process(filtered);
		cv::Mat scale = gsp.GetScaleMap();
		cv::Mat scaleImg = gsp.GetScaleImage();
		cv::Mat normScale;
		cv::normalize(scale, normScale, 0.0, 1.0, cv::NORM_MINMAX);
		//cv::imshow("scaleImg", scaleImg);
		//cv::imshow("scale", normScale);
		//cv::waitKey(33);

		//cv::equalizeHist(filtered, filtered);
		ComputeGradient(filtered, parameters->K*2 + 1);

		double dominantOrientation = parameters->Orientation;
		if (!parameters->UseProvidedOrientation)
		{
			dominantOrientation = ComputeDominantOrientation(DominantOrientationMethod::HOG, parameters->DominantOrientationBins);
		}
		
		//dominantOrientation += 3.1416;
		std::cout << "Dominant Orientation: " << dominantOrientation << std::endl;
		//double dominantOrientation = -0.6;

		ComputeDirectionImage(outputImg, dominantOrientation, scale);

		//cv::equalizeHist(outputImg, outputImg);
		//int blockSize = parameters->K * 12 + 1;
		//double C = (double)blockSize / -5.0;
		//cv::adaptiveThreshold(outputImg, outputImg, 255.0, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, blockSize, C);

		//cv::imshow("outputImg", outputImg);
		//cv::waitKey(0);

	}

	void EdgeDirectionImageProcessor::ComputeDirectionImage(cv::Mat &output, double dominantOrientation, const cv::Mat &scale)
	{
		output = cv::Mat(BaseData.Gradient.rows, BaseData.Gradient.cols, CV_8U);

		cv::Mat direction(BaseData.Gradient.rows, BaseData.Gradient.cols, CV_8U);

		/*cv::Mat normGrad;
		BaseData.toImage(GRADIENT_MAT_MAGNITUDE_INDEX).convertTo(normGrad, CV_64F);
		cv::normalize(normGrad, normGrad, 0.5, 1.0, cv::NORM_MINMAX);*/
		
		//cv::Mat directionMap = BaseData.toImage(GRADIENT_MAT_DIRECTION_INDEX);

		output = cv::Mat(BaseData.Gradient.rows, BaseData.Gradient.cols, CV_8U);

		cv::Vec2d directionVector;
		directionVector[0] = std::cos(dominantOrientation);
		directionVector[1] = std::sin(dominantOrientation);

		for (int x = 0; x < output.cols; ++x)
		{
			for (int y = 0; y < output.rows; ++y)
			{
				//double d = directionMap.at<double>(y, x);
				//double low = fabs(d - dominantOrientation);
				//double high = fabs(d - (dominantOrientation + 2.0*3.1416));
				//double val = (std::min(low, high) / 3.1416); /**normGrad.at<double>(y,x)*/;
				//direction.at<uchar>(y, x) = (uchar)(val * 255.0);
				//if (val >= 0.5)
				//	output.at<uchar>(y, x) = 0;
				//else
				//	output.at<uchar>(y, x) = 255;

				cv::Vec2d d;
				d[0] = BaseData.getXDerivAt(x, y);
				d[1] = BaseData.getYDerivAt(x, y);
				
				double norm = std::sqrt(d[0] * d[0] + d[1] * d[1]);
				if (norm <= 0.00001)
					norm = 1.0;
				d[0] /= norm;
				d[1] /= norm;

				double dot = directionVector.dot(d);
				//std::cout << dot << std::endl;
				if (dot >= 0.0)
					output.at<uchar>(y, x) = 255;
				else
					output.at<uchar>(y, x) = 0;
			}
		}

		//output = direction;
		//cv::erode(output, output, cv::Mat(), cv::Point(-1, -1), 2);
		//cv::medianBlur(output, output, parameters->K);
	}
}