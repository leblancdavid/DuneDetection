#include "FrequencySpaceImageProcessor.h"
#include "GenericImageProcessing.h"

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
		int k = 5;
		cv::Mat filtered, eq, hpf;
		cv::medianBlur(inputImg, filtered, k);
		cv::GaussianBlur(filtered, filtered, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);

		cv::equalizeHist(filtered, eq);
		ComputeGradient(eq, 5);

		cv::Mat spectrum;
		
		double domDir = ComputeDominantOrientation(DominantOrientationMethod::K_MEANS, 2);
		std::cout << domDir << std::endl;

		//cv::Mat threshold;
		//cv::adaptiveThreshold(filtered, threshold, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 201, -80.0);

		//cv::Mat edgeImg = BaseData.toImage(GRADIENT_MAT_MAGNITUDE_INDEX);
		ComputeDFT(eq, spectrum);
		//ApplyHighPassFilter(spectrum, outputImg);
		ApplyDFTFiltering(spectrum, hpf);

		//sum up the images to basically enhance the images
		cv::Mat temp1, temp2;
		filtered.convertTo(temp1, CV_64F);
		hpf.convertTo(temp2, CV_64F, 255.0);


		outputImg = temp1 + temp2;
		outputImg.convertTo(outputImg, CV_8U, 0.5);

		
		//cv::Mat test;
		//filtered.convertTo(test, CV_64F, 1.0 / 255.0);
		//outputImg = outputImg + test;
		//outputImg.convertTo(outputImg, CV_8U, 255.0);

		//cv::threshold(outputImg, outputImg, 1, 255, CV_THRESH_BINARY_INV);
		cv::imshow("filtered", eq);
		cv::imshow("outputImg", outputImg);
		cv::waitKey(0);
		outputImg = inputImg.clone();
	}


	void FrequencySpaceImageProcessor::ComputeDFT(const cv::Mat &inputImg, cv::Mat &outputImg)
	{
		//cv::Mat gradientImg = BaseData.toImage(GRADIENT_MAT_MAGNITUDE_INDEX);
		//cv::imshow("gradientImg", gradientImg);
		//cv::waitKey(0);
		cv::Mat spectrum;
		DFTProcess.Forward(inputImg, spectrum);
		spectrum.convertTo(outputImg, CV_8U, 255.0);
	}

	void FrequencySpaceImageProcessor::ApplyDFTFiltering(const cv::Mat &spectrum, cv::Mat &outputImg)
	{
		cv::RotatedRect elipse;
		FitEllipseToDFT(spectrum, elipse);

		//Make the elipse a circle?
		elipse.size.width /= 33.0;
		elipse.size.height /= 33.0;
		

		//test high pass filter
		DFTProcess.HighPassFilter(elipse, outputImg);
		//cv::Mat mask(spectrum.size(), CV_64F);
		//mask = cv::Scalar(1.0);
		//DFTProcess.MaskFilter(mask, outputImg);
		//outputImg.convertTo(outputImg, CV_8U, 255.0);
	}

	void FrequencySpaceImageProcessor::FitEllipseToDFT(const cv::Mat &dftImg, cv::RotatedRect &elipse)
	{
		cv::Scalar mean, stddev;
		cv::meanStdDev(dftImg, mean, stddev);

		cv::Mat threshold;
		cv::threshold(dftImg, threshold, mean[0] + 3.0*stddev[0], 255, CV_THRESH_BINARY);
		cv::medianBlur(threshold, threshold, 11);
		//cv::imshow("threshold", threshold);
		//cv::waitKey(0);

		std::vector<std::vector<cv::Point>> contours;

		cv::findContours(threshold, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		if (contours.empty())
		{
			elipse = cv::RotatedRect();
			return;
		}

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
		//cv::imshow("dftImg", dftImg);
		//cv::imshow("elipse fit", tempColor);
		//cv::waitKey(0);
	}


	void FrequencySpaceImageProcessor::ApplyHighPassFilter(const cv::Mat &spectrum, cv::Mat &outputImg)
	{
		cv::Scalar mean, stddev;
		cv::meanStdDev(spectrum, mean, stddev);

		cv::Mat threshold;
		cv::threshold(spectrum, threshold, mean[0] + 3.0*stddev[0], 255, CV_THRESH_BINARY_INV);
		int k = 11;
		//cv::medianBlur(threshold, threshold, 11);
		cv::GaussianBlur(threshold, threshold, cv::Size(k, k), 1.8, 1.8);
		//cv::imshow("threshold", threshold);
		//cv::waitKey(0);
		DFTProcess.MaskFilter(threshold, outputImg);

		outputImg.convertTo(outputImg, CV_8U, 255.0);
	}

	void FrequencySpaceImageProcessor::DirectionalSharpening(const cv::Mat &input, cv::Mat &output, double direction)
	{
		int k = 9;
		cv::Mat derivKernel2, derivKernel1, rotatedK;

		//First figure out how big we need to make the masks
		double r = (double)k / 2.0;
		r = 2.0 * std::sqrt(r*r * 2.0);
		int size = std::ceil(r);

		cv::getDerivKernels(derivKernel2, derivKernel1, 2, 0, k, false, CV_32F);

		//Rotate the kernel
		double angle = direction * 180.0 / 3.1416;
		cv::Mat rotMat = cv::getRotationMatrix2D(cv::Point2f((float)size / 2.0f, (float)size / 2.0f), angle, 1.0);
		cv::warpAffine(derivKernel2, rotatedK, rotMat, cv::Size(size, size));

		cv::Mat filteredImg;
		cv::filter2D(input, filteredImg, CV_32F, rotatedK);
		
		cv::normalize(filteredImg, output, 0, 1, cv::NORM_MINMAX);
		/*cv::Mat temp;
		input.convertTo(temp, CV_32F);
		output = filteredImg + temp;
		cv::normalize(output, output, 0, 1, cv::NORM_MINMAX);*/

		//cv::imshow("filteredImg", output);
		//cv::waitKey(0);
	}
}