#ifndef _DUNE_METRIC_CALCULATOR_H_
#define _DUNE_METRIC_CALCULATOR_H_

#include "OpenCVHeaders.h"
#include "ConnectedComponentsExtractor.h"

namespace dune
{
	namespace imgproc
	{
		struct DuneMetrics
		{
		public:
			double AverageOrientation;
			double AverageDuneSpacing;
		};

		class DuneMetricCalculator
		{
		public:
			DuneMetricCalculator() 
			{
				epsilon = 2.5;
			}
			~DuneMetricCalculator() {}

			DuneMetrics CalculateMetrics(const cv::Mat &binaryImage)
			{
				ConnectedComponentsExtractor cce;
				std::vector<std::vector<cv::Point>> segments = cce.GetCC(binaryImage);

				cv::Mat lineImage;
				cv::cvtColor(binaryImage, lineImage, CV_GRAY2BGR);

				cv::Vec2d averageOrientationVector;
				averageOrientationVector[0] = 0;
				averageOrientationVector[1] = 0;
				for (size_t i = 0; i < segments.size(); ++i)
				{
					std::vector<cv::Point> approx;
					cv::approxPolyDP(segments[i], approx, epsilon, false);

					if (approx.size() <= 1)
						continue;
					
					for (size_t j = 1; j < approx.size(); ++j)
					{
						cv::Vec2d dir;
						dir[0] = approx[j].x - approx[j - 1].x;
						dir[1] = approx[j].y - approx[j - 1].y;
						if (dir[1] < 0)
						{
							dir[0] *= -1.0;
							dir[1] *= -1.0;
						}
						averageOrientationVector += dir;
						cv::line(lineImage, approx[j - 1], approx[j], cv::Scalar(0, 255, 0), 2);
					}
				}

				DuneMetrics metrics;
				metrics.AverageOrientation = std::atan2(averageOrientationVector[1], averageOrientationVector[0]);// *180.0 / 3.1416;
				metrics.AverageDuneSpacing = GetAverageDuneSpacing(binaryImage, metrics.AverageOrientation - 1.5708); //cross angle is -90 degrees
				//cv::imshow("lines", lineImage);
				//cv::waitKey(0);

				return metrics;
			}
		private:

			double GetAverageDuneSpacing(const cv::Mat &binaryImage, double crossAngle)
			{
				if (crossAngle < -1.5708)
					crossAngle += 3.1416;
				else if (crossAngle > 1.5708)
					crossAngle -= 3.1416;
				int segments = 0;
				int totalSpacing = 0;
				for (double i = -1 * binaryImage.rows; i < binaryImage.rows * 2; ++i)
				{
					bool beginSearch = false;
					bool searching = false;
					int spaceLength = 0;
					for (int j = 0; j < binaryImage.cols; ++j)
					{
						int y = std::ceil(std::tan(crossAngle)*(double)j + i - 0.5);
						
						cv::Point pt(j, y);
						if (pt.y < 0 || pt.y >= binaryImage.rows)
							continue;

						if (binaryImage.at<uchar>(pt))
						{
							if (!beginSearch)
							{
								beginSearch = true;
							}
							else
							{
								if (searching)
								{
									segments++;
									totalSpacing += spaceLength;
									spaceLength = 0;
									searching = false;
								}
							}
						}
						else
						{
							if (beginSearch)
							{
								if (!searching)
									searching = true;
								spaceLength++;
							}
						}


					}
				}

				return (double)totalSpacing / (double)segments;
			}

			double epsilon;
		};
	}

}


#endif
