#ifndef _DUNE_REGION_THINNER_H_
#define _DUNE_REGION_THINNER_H_

#include "BaseDuneRegionThinner.h"

namespace duneML
{

	class DuneRegionThinner : public BaseDuneRegionThinner
	{
	public:


		DuneRegionThinner() 
		{
			lowerThreshold = -0.5;
			upperThreshold = 0.5;
			smoothing = 31;
			
		}
		~DuneRegionThinner() {}

		//Expects a CV_32F Mat
		std::vector<dune::DuneSegment> GetDuneSegments(const cv::Mat &input, const std::string &saveFileName="")
		{
			cv::Mat processed;
			if (smoothing > 0 && smoothing % 1)
			{
				cv::GaussianBlur(input, processed, cv::Size(smoothing, smoothing), smoothing / 3.0, smoothing / 3.0);
			}
			else
			{
				processed = input;
			}

			//cv::Mat x_k, y_k, xDerivImage, yDerivImage, magnitudeImage;
			//Get2ndOrderKernels(smoothing, x_k, y_k);
			//cv::filter2D(processed, xDerivImage, CV_64F, x_k);
			//cv::filter2D(processed, yDerivImage, CV_64F, y_k);
			//magnitudeImage = cv::Mat(processed.rows, processed.cols, CV_64F);
			/*for (int i = 0; i < processed.rows; ++i)
			{
				for (int j = 0; j < processed.cols; ++j)
				{
					magnitudeImage.at<double>(i, j) = std::sqrt(xDerivImage.at<double>(i, j)*xDerivImage.at<double>(i, j) +
						yDerivImage.at<double>(i, j)*yDerivImage.at<double>(i, j));									
				}
			}*/


			std::vector<cv::Point> anchors = GetAnchorPoints(processed);

			cv::Mat outputImage(processed.rows, processed.cols, CV_8U);
			outputImage = cv::Scalar(0);

			std::vector<dune::DuneSegment> segments;

			for (int i = 0; i < anchors.size(); ++i)
			{
				std::vector<cv::Point> pointList;

				if (outputImage.at<uchar>(anchors[i]) != 0)
					continue;

				RecursiveTrace(anchors[i], processed, outputImage, pointList);

				segments.push_back(GetDuneSegmentFromPointList(pointList));
			}

			if (saveFileName != "")
			{
				cv::imwrite(saveFileName + "_thin_results.jpg", outputImage);
				cv::Mat smoothResults = outputImage.clone();
				smoothResults = cv::Scalar(0);
				for (size_t i = 0; i < segments.size(); ++i)
				{
					for (size_t j = 0; j < segments[i].GetLength(); ++j)
					{
						smoothResults.at<uchar>(segments[i][j].Position) = 255;
					}
				}
				cv::imwrite(saveFileName + "_thin_results_smooth.jpg", smoothResults);
			}

			return segments;
		}

	private:
		float lowerThreshold;
		float upperThreshold;
		int smoothing;

		std::vector<cv::Point> GetAnchorPoints(const cv::Mat &input)
		{
			std::vector<cv::Point> anchors;
			for (int x = 1; x < input.cols - 1; ++x)
			{
				for (int y = 1; y < input.rows - 1; ++y)
				{
					if (input.at<float>(y, x) < upperThreshold)
						continue;

					if (IsAnchorPoint(input, x, y))
						anchors.push_back(cv::Point(x, y));
				}
			}
			return anchors;
		}

		bool IsAnchorPoint(const cv::Mat &input, int x, int y)
		{
			float anchorVal = input.at<float>(y, x);
			for (int i = -1; i <= 1; ++i)
			{
				for (int j = -1; j <= 1; ++j)
				{
					if (i == 0 && j == 0)
						continue;

					if (input.at<float>(y + j, x + i) > anchorVal)
						return false;
				}
			}

			return true;
		}

		std::vector<cv::Point> GetNextNeighbors(const cv::Point &anchor, const cv::Mat &input)
		{
			std::vector <cv::Point> neighbors;
			if (anchor.x == 0 || anchor.x == input.cols - 1 ||
				anchor.y == 0 || anchor.y == input.rows - 1)
				return neighbors;

			neighbors.push_back(cv::Point(anchor.x - 1, anchor.y - 1));
			neighbors.push_back(cv::Point(anchor.x, anchor.y - 1));
			neighbors.push_back(cv::Point(anchor.x + 1, anchor.y - 1));
			neighbors.push_back(cv::Point(anchor.x + 1, anchor.y));
			neighbors.push_back(cv::Point(anchor.x + 1, anchor.y + 1));
			neighbors.push_back(cv::Point(anchor.x, anchor.y + 1));
			neighbors.push_back(cv::Point(anchor.x - 1, anchor.y + 1));
			neighbors.push_back(cv::Point(anchor.x - 1, anchor.y));

			std::vector<cv::Point> peaks;

			double anchorVal = input.at<double>(anchor);

			for (int i = 0; i < 8; ++i)
			{
				int l = i - 1 ;
				int r = i + 1;
				if (l < 0)
				{
					l = 7;
				}
				if (r >= 8)
				{
					r = 0;
				}

				double center = input.at<double>(neighbors[i]);
				double left = input.at<double>(neighbors[l]);
				double right = input.at<double>(neighbors[r]);

				if (center < left && center < right && center > lowerThreshold)
				{
					peaks.push_back(neighbors[i]);
				}
			}

			return peaks;
		}

		void RecursiveTrace(const cv::Point anchor, const cv::Mat &input, cv::Mat &trace, std::vector<cv::Point> &pointList)
		{
			std::vector<cv::Point> neighbors = GetNextNeighbors(anchor, input);

			trace.at<uchar>(anchor) = 255;
			pointList.push_back(anchor);
			for (size_t i = 0; i < neighbors.size(); ++i)
			{
				if (trace.at<uchar>(neighbors[i]) != 0)
					continue;

				//trace.at<uchar>(neighbors[i]) = 255;

				RecursiveTrace(neighbors[i], input, trace, pointList);
			}
		}

		dune::DuneSegment GetDuneSegmentFromPointList(const std::vector<cv::Point> &pointList)
		{
			std::vector<cv::Point> sortedList = GetSortedPointList(pointList);

			cv::Mat gausKernel = cv::getGaussianKernel(smoothing, (double)smoothing / 3.0);

			std::vector<cv::Point2d> smoothedList(sortedList.size());
			std::vector<dune::DuneSegmentData> segmentData(sortedList.size());
			int k = gausKernel.rows / 2;
			for (int i = 0; i < smoothedList.size(); ++i)
			{
				smoothedList[i].x = 0.0;
				smoothedList[i].y = 0.0;
				for (int j = 0; j < gausKernel.rows; ++j)
				{
					int index = i + j - k;
					if (index < 0)
						index = 0;
					if (index >= sortedList.size())
						index = sortedList.size() - 1;
					smoothedList[i].x += gausKernel.at<double>(j, 0) * (double)sortedList[index].x;
					smoothedList[i].y += gausKernel.at<double>(j, 0) * (double)sortedList[index].y;
				}

				segmentData[i].Position = smoothedList[i];
			}

			dune::DuneSegment segment;
			segment.SetSegmentData(segmentData);

			return segment;
		}

		std::vector<cv::Point> GetSortedPointList(const std::vector<cv::Point> &pointList)
		{
			cv::Vec4f line;
			cv::fitLine(pointList, line, CV_DIST_L2, 0, 0.01, 0.01);

			cv::Point2f anchor(line[2] + line[0] * (float)pointList.size(),
				line[3] + line[1] * (float)pointList.size());

			std::vector<cv::Point> sortedPointList(pointList.size());
			std::vector<float> distances(pointList.size());

			//compute the distances from the anchor point
			for (size_t i = 0; i < pointList.size(); ++i)
			{
				distances[i] = std::sqrt((anchor.x - pointList[i].x)*(anchor.x - pointList[i].x) +
					(anchor.y - pointList[i].y)*(anchor.y - pointList[i].y));
			}

			//get the sorted list based on the distances

			for (size_t i = 0; i < sortedPointList.size(); ++i)
			{
				float minDist = FLT_MAX;
				size_t minIndex = 0;
				for (size_t j = 0; j < distances.size(); ++j)
				{
					if (distances[j] < minDist)
					{
						minIndex = j;
						minDist = distances[j];
					}
				}

				sortedPointList[i] = pointList[minIndex];
				distances[minIndex] = FLT_MAX;
			}

			return sortedPointList;
		}

		void Get2ndOrderKernels(int size, cv::Mat &x, cv::Mat &y)
		{
			cv::Mat derivX, derivY;
			cv::getDerivKernels(derivX, derivY, 2, 2, size, true, CV_64F);

			//transpose because for some reason opencv makes the x-deriv a size-by-1 Mat
			derivX = derivX.t();

			x = cv::Mat(size, size, CV_64F); 
			y = cv::Mat(size, size, CV_64F);

			for (int i = 0; i < size; ++i)
			{
				x.row(i) = derivX;
				y.col(i) = derivY;
			}
		}

	};
	


}

#endif