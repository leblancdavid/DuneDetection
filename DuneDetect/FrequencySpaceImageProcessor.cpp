#include "FrequencySpaceImageProcessor.h"

namespace dune
{
	FrequencySpaceImageProcessor::FrequencySpaceImageProcessor()
	{


	}

	FrequencySpaceImageProcessor::~FrequencySpaceImageProcessor()
	{

	}

	FrequencySpaceImageProcessor::FrequencySpaceImageProcessor(const FrequencySpaceImageProcessor &cpy)
	{

	}

	void FrequencySpaceImageProcessor::Process(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		int k = 9;
		cv::Mat filtered;
		cv::medianBlur(inputImg, filtered, k);
		cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);

		cv::Mat spectrum;
		ComputeDFT(filtered, spectrum);

		cv::imwrite("DFT_spectrum.jpg", spectrum);

		ApplyDFTFiltering(spectrum, outputImg);



		outputImg = inputImg.clone();
	}


	void FrequencySpaceImageProcessor::ComputeDFT(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		cv::Mat spectrum;
		DFTProcess.Forward(inputImg, spectrum);
		spectrum.convertTo(outputImg, CV_8U, 255.0);
	}

	void FrequencySpaceImageProcessor::ApplyDFTFiltering(const cv::Mat &spectrum, cv::Mat &outputImg)
	{
		cv::RotatedRect elipse;
		FitEllipseToDFT(spectrum, elipse);

		//Make the elipse a circle?
		elipse.size.height /= 3.0;
		elipse.size.width /= 3.0;

		//test high pass filter
		DFTProcess.HighPassFilter(elipse, outputImg);
		outputImg.convertTo(outputImg, CV_8U, 255.0);

		cv::imwrite("DFT_HPF.jpg", outputImg);
		cv::imshow("outputImg", outputImg);
		cv::waitKey(0);
	}

	void FrequencySpaceImageProcessor::FitEllipseToDFT(const cv::Mat &dftImg, cv::RotatedRect &elipse)
	{
		cv::Scalar mean, stddev;
		cv::meanStdDev(dftImg, mean, stddev);

		cv::Mat threshold;
		cv::threshold(dftImg, threshold, mean[0] + 3.5*stddev[0], 255, CV_THRESH_BINARY);
		cv::imwrite("DFT_spectrum_threshold.jpg", threshold);
		cv::medianBlur(threshold, threshold, 21);
		cv::imwrite("DFT_spectrum_threshold_median.jpg", threshold);

		std::vector<std::vector<cv::Point>> contours;

		cv::findContours(threshold, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		int largest = 0, index;
		for (size_t i = 0; i < contours.size(); ++i)
		{
			if (contours[i].size() > largest)
			{
				largest = contours[i].size();
				index = i;
			}
		}

		elipse = cv::fitEllipse(contours[index]);

		//elipse.angle += 90;

		cv::Mat tempColor;
		cv::cvtColor(dftImg, tempColor, CV_GRAY2BGR);
		cv::ellipse(tempColor, elipse, cv::Scalar(0, 0, 255), 2);
		cv::imwrite("DFT_spectrum_ellipsefit.jpg", tempColor);
		//cv::imshow("dftImg", dftImg);
		//cv::imshow("elipse fit", tempColor);
		//cv::waitKey(0);
	}
}