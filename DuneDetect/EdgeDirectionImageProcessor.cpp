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

		//cv::adaptiveBilateralFilter(filtered, bilateral, cv::Size(5, 5), 1.5);
		//cv::GaussianBlur(filtered, filtered, cv::Size(5, 5), 1.5, 1.5);


		GaussianScalePyramid gsp;
		gsp.Process(filtered);
		cv::Mat scale = gsp.GetScaleMap();
		cv::Mat scaleImg = gsp.GetScaleImage();

		cv::imshow("scaleImg", scaleImg);
		cv::waitKey(33);

		//cv::equalizeHist(scaleImage, filtered);
		ComputeGradient(scaleImg, parameters->K * 2 + 1);

		//double dominantOrientation = ComputeDominantOrientation(DominantOrientationMethod::HOG, parameters->DominantOrientationBins);
		double dominantOrientation = -0.6;

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

		cv::Mat direction(BaseData.Gradient.rows, BaseData.Gradient.cols, CV_64F);

		cv::Mat normGrad;
		BaseData.toImage(GRADIENT_MAT_MAGNITUDE_INDEX).convertTo(normGrad, CV_64F);
		cv::normalize(normGrad, normGrad, 0.5, 1.0, cv::NORM_MINMAX);
		
		cv::Mat directionMap = BaseData.toImage(GRADIENT_MAT_DIRECTION_INDEX, scale);

		for (int x = 0; x < directionMap.cols; ++x)
		{
			for (int y = 0; y < directionMap.rows; ++y)
			{
				double d = directionMap.at<double>(y, x);
				double low = fabs(d - dominantOrientation);
				double high = fabs(d - (dominantOrientation + 2.0*3.1416));

				/*uchar pixelVal;
				if (low < high)
				{
					pixelVal = (uchar)std::ceil(low / 3.1416 * 255.0 - 0.5);
				}
				else
				{
					pixelVal = (uchar)std::ceil(high / 3.1416 *255.0 - 0.5);
				}

				if (pixelVal > 255)
				{
					std::cout << "BAD: " << pixelVal << std::endl;
					pixelVal = 255;
				}
				output.at<uchar>(y, x) = pixelVal;*/

				double val = (std::min(low, high) / 3.1416); /**normGrad.at<double>(y,x)*/;
				direction.at<double>(y, x) = val;
			}
		}

		cv::normalize(direction, direction, 0.0, 255.0, cv::NORM_MINMAX);
		direction.convertTo(output, CV_8U);
	}
}