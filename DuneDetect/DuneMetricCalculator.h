#ifndef _DUNE_METRIC_CALCULATOR_H_
#define _DUNE_METRIC_CALCULATOR_H_

#include "OpenCVHeaders.h"
#include "ConnectedComponentsExtractor.h"
#include "SkeletonizationRegionThinner.h"
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
				epsilon = 10.0;
			}
			~DuneMetricCalculator() {}

			DuneMetrics CalculateMetrics(const cv::Mat &binaryImage)
			{
				/**/
				DuneMetrics metrics;
				//metrics.AverageOrientation = std::atan2(averageOrientationVector[1], averageOrientationVector[0]);// *180.0 / 3.1416;

				std::vector<double> histogram;
				metrics.AverageOrientation = GetAverageOrientation(binaryImage, histogram) * 180.0 / 3.1416;
				metrics.AverageDuneSpacing = GetAverageDuneSpacing(binaryImage, metrics.AverageOrientation - 1.5708); //cross angle is -90 degrees
				//cv::imshow("lines", lineImage);
				//cv::waitKey(0);

				return metrics;
			}
		private:

			std::vector<double> GetAngles3(const cv::Mat &binaryImage, std::vector<double> &weights)
			{
				std::vector<double> angles;

				cv::Mat xderiv, yderiv;
				cv::Sobel(binaryImage, xderiv, CV_64F, 1, 0, 5);
				cv::Sobel(binaryImage, yderiv, CV_64F, 0, 1, 5);

				for (int i = 0; i < binaryImage.rows; ++i)
				{
					for (int j = 0; j < binaryImage.cols; ++j)
					{
						if (binaryImage.at<uchar>(i, j) == 0)
							continue;

						double dx = xderiv.at<double>(i, j);
						double dy = yderiv.at<double>(i, j);

						double angle = std::atan2(dy, dx) + CV_PI/2.0;
						while (angle < 0.0)
							angle += CV_PI;
						while (angle > CV_PI)
							angle -= CV_PI;

						angles.push_back(angle);

					}
				}

				return angles;
			}


			std::vector<double> GetAngles2(const cv::Mat &binaryImage, std::vector<double> &weights)
			{
				std::vector<double> angles;
				std::vector<cv::Vec4i> lines;

				cv::HoughLinesP(binaryImage, lines, 1, CV_PI / 180.0, 20, 10, 50.0);

				cv::Mat lineImage;
				cv::cvtColor(binaryImage, lineImage, CV_GRAY2BGR);
				for (size_t i = 0; i < lines.size(); i++)
				{
					line(lineImage, cv::Point(lines[i][0], lines[i][1]),
						cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0, 0, 255), 3, 8);

					double angle = std::atan2(lines[i][3] - lines[i][1], lines[i][2] - lines[i][1]);
					while (angle < 0.0)
						angle += CV_PI;
					while (angle > CV_PI)
						angle -= CV_PI;

					angles.push_back(angle);
				}

				cv::imshow("lineImage", lineImage);
				cv::waitKey(100);

				return angles;
			}

			std::vector<double> GetAngles(const cv::Mat &binaryImage, std::vector<double> &weights)
			{
				ConnectedComponentsExtractor cce;
				std::vector<std::vector<cv::Point>> segments = cce.GetCC(binaryImage);

				cv::Mat lineImage;
				cv::cvtColor(binaryImage, lineImage, CV_GRAY2BGR);

				std::vector<double> angles;
				weights.clear();

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

						weights.push_back((dir[0] * dir[0] + dir[1] * dir[1]) / 1000.0);
						angles.push_back(std::atan2(dir[1], dir[0]));

						cv::line(lineImage, approx[j - 1], approx[j], cv::Scalar(0, 255, 0), 2);
					}
				}

				cv::imshow("lineImage", lineImage);
				cv::waitKey(10);

				return angles;
			}

			double GetAverageOrientation(const cv::Mat &binaryImage, std::vector<double> &histogram)
			{
				std::vector<double> weights;
				std::vector<double> angles = GetAngles(binaryImage, weights);

				double m1 = 0.0, m2 = 0.0;
				double n = 0;
				for (int i = 0; i < angles.size(); ++i)
				{
					m1 += angles[i];
					m2 += angles[i] * angles[i];
					n++;
				}

				double meanAngle, minMeanAngle, delta, minDelta = DBL_MAX;
				for (int i = 1; i < angles.size(); ++i)
				{
					m1 += CV_PI;
					m2 += 2.0*CV_PI*angles[i] + CV_PI*CV_PI;

					meanAngle = m1 / n;
					delta = m2 - 2.0*m1*meanAngle + n*meanAngle*meanAngle;
					if (delta < minDelta)
					{
						minMeanAngle = meanAngle;
						minDelta = delta;
					}

				}

				while (minMeanAngle < 0) 
				{
					minMeanAngle += 3.1416;
				}
				while (minMeanAngle > 3.1416)
				{
					minMeanAngle -= 3.1416;
				}
				

				return minMeanAngle;
				/*cv::Mat dx, dy;
				cv::getDerivKernels(dx, dy, 2, 2, 7);
				cv::resize(dx, dx, cv::Size(7, 7));
				cv::resize(dy, dy, cv::Size(7, 7));
				dx = -1.0 * dx;
				dy = -1.0 * dy;
				dx = dx.t();

				cv::Mat dxImg, dyImg;
				cv::filter2D(binaryImage, dxImg, CV_64F, dx);
				cv::filter2D(binaryImage, dyImg, CV_64F, dy);

				double xSum = 0.0;
				double ySum = 0.0;
				for (int i = 0; i < binaryImage.rows; ++i)
				{
					for (int j = 0; j < binaryImage.cols; ++j)
					{
						if (binaryImage.at<uchar>(i, j) == 0)
							continue;

						xSum += dxImg.at<double>(i, j) / 100.0;
						ySum += dyImg.at<double>(i, j) / 100.0;
					}
				}

				return std::atan2(ySum, xSum);*/
			}

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
