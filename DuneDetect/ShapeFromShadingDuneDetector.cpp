#include "ShapeFromShadingDuneDetector.h"

namespace dune
{
	ShapeFromShadingDetector::ShapeFromShadingDetector()
	{

	}

	ShapeFromShadingDetector::ShapeFromShadingDetector(BaseImageProcessor* imgproc, ShapeFromShadingDetectorParameters *params)
	{
		ImageProcess = imgproc;
		Parameters = params;
	}

	ShapeFromShadingDetector::~ShapeFromShadingDetector()
	{

	}

	std::vector<DuneSegment> ShapeFromShadingDetector::Extract(const cv::Mat &img)
	{
		cv::Mat processedImage;
		ImageProcess->Process(img, processedImage);

		std::vector<DuneSegment> segments;
		cv::Mat peaks = cv::Mat::zeros(img.size(), CV_8U);

		for (int i = 1; i < img.rows - 1; ++i)
		{
			for (int j = 1; j < img.cols - 1; ++j)
			{
				if ((processedImage.at<float>(i, j) > processedImage.at<float>(i, j - 1) &&
					processedImage.at<float>(i, j) > processedImage.at<float>(i, j + 1)) ||
					(processedImage.at<float>(i, j) > processedImage.at<float>(i - 1, j) &&
					processedImage.at<float>(i, j) > processedImage.at<float>(i + 1, j)))
				{
					peaks.at<uchar>(i, j) = 255;
				}
			}
		}

		cv::imwrite("sampleOutput.png", peaks);
		cv::imshow("peaks", peaks);
		cv::waitKey(0);

		return segments;
	}
}