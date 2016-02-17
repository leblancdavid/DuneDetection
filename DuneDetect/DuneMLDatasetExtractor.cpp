#include "DuneMLDatasetExtractor.h"

namespace duneML
{

	void DuneMLDatasetExtractor::GetDatasetPoints(const cv::Mat &groundTruthImage,
		std::vector<cv::KeyPoint> &positive, std::vector<cv::KeyPoint> &negative)
	{
		cv::Mat posImg = groundTruthImage.clone();
		cv::Mat negImg;
		//dilate so we have a bit of buffer between positve and negative examples.
		//cv::dilate(groundTruthImage, posImg, cv::Mat(), cv::Point(-1, -1), parameters.BufferDistancePixels/2);
		cv::dilate(groundTruthImage, negImg, cv::Mat(), cv::Point(-1, -1), parameters.BufferDistancePixels);
		
		//cv::imshow("negImg", negImg);
		//cv::waitKey(0);
		
		positive.clear();
		for (int i = 0; i < parameters.NumPositiveExamples; ++i)
		{
			uchar pixelVal = 0;
			cv::KeyPoint pt;
			do
			{
				pt.pt.x = rand() % (posImg.cols-1);
				pt.pt.y = rand() % (posImg.rows-1);
				pixelVal = posImg.at<uchar>(pt.pt);
			} while (pixelVal == 0);
			//mark the image point
			posImg.at<uchar>(pt.pt) = 0;
			positive.push_back(pt);
		}

		negative.clear();
		for (int i = 0; i < parameters.NumPositiveExamples; ++i)
		{
			uchar pixelVal = 0;
			cv::KeyPoint pt;
			do
			{
				pt.pt.x = rand() % (negImg.cols-1);
				pt.pt.y = rand() % (negImg.rows-1);
				pixelVal = negImg.at<uchar>(pt.pt);
			} while (pixelVal == 1);
			//mark the image point
			negImg.at<uchar>(pt.pt) = 1;
			negative.push_back(pt);
		}
	}

}