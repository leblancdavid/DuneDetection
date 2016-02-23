#include "SuperDuperImageProcessor.h"
#include "GenericImageProcessing.h"
#include "GaussianScalePyramid.h"

namespace dune
{
	SuperDuperImageProcessor::SuperDuperImageProcessor()
	{
		parameters = new SuperDuperParams();
	}

	SuperDuperImageProcessor::~SuperDuperImageProcessor()
	{
		delete parameters;
	}

	SuperDuperImageProcessor::SuperDuperImageProcessor(const SuperDuperImageProcessor &cpy)
	{
		parameters = cpy.parameters;
	}

	SuperDuperImageProcessor::SuperDuperImageProcessor(SuperDuperParams *params)
	{
		parameters = params;
	}

	void SuperDuperImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		int k = parameters->K;
		cv::Mat filtered, bilateral, threshold, canny;
		cv::medianBlur(inputImg, filtered, parameters->K);
		//cv::bilateralFilter(filtered, bilateral, cv::Size(5, 5), 1.5);

		GaussianScalePyramid gsp;
		gsp.Process(filtered);
		cv::Mat scale = gsp.GetScaleMap();

		ComputeGradient(bilateral, parameters->K * 2 + 1);

		double dominantOrientation = ComputeDominantOrientation(DominantOrientationMethod::HOG, parameters->DominantOrientationBins);

		cv::Mat direction, gradient;
		ComputeDirectionImage(direction, dominantOrientation, scale);
		GetGradientImage(gradient);

		std::vector<cv::Mat> output;
		output.push_back(bilateral);
		output.push_back(direction);
		output.push_back(gradient);

		cv::merge(output, outputImg);
	}

	void SuperDuperImageProcessor::ComputeDirectionImage(cv::Mat &output, double dominantOrientation, const cv::Mat &scale)
	{
		output = cv::Mat(BaseData.Gradient.rows, BaseData.Gradient.cols, CV_8U);

		cv::Mat direction(BaseData.Gradient.rows, BaseData.Gradient.cols, CV_64F);
		cv::Mat directionMap = BaseData.toImage(GRADIENT_MAT_DIRECTION_INDEX, scale);

		for (int x = 0; x < directionMap.cols; ++x)
		{
			for (int y = 0; y < directionMap.rows; ++y)
			{
				double d = directionMap.at<double>(y, x);
				double low = fabs(d - dominantOrientation);
				double high = fabs(d - (dominantOrientation + 2.0*3.1416));

				double val = std::min(low, high);
				direction.at<double>(y, x) = val;
			}
		}

		cv::normalize(direction, direction, 0.0, 255.0, cv::NORM_MINMAX);
		direction.convertTo(output, CV_8U);

		//cv::equalizeHist(output, output);
	}

	void SuperDuperImageProcessor::GetGradientImage(cv::Mat &output)
	{
		output = BaseData.toImage(GRADIENT_MAT_MAGNITUDE_INDEX);
		//cv::equalizeHist(output, output);
	}
}