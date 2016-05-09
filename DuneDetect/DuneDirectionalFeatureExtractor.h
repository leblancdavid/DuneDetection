#pragma once

#include "BaseFeatureExtractor.h"
#include "OpenCVHeaders.h"

#include <list>
namespace duneML
{
	class DirectionalFeaturesParameters
	{
	public:

		DirectionalFeaturesParameters()
		{
			WindowSize = 13;
			descriptorLength = 31;
			K = 31;
		}
		~DirectionalFeaturesParameters() {}
		DirectionalFeaturesParameters(const DirectionalFeaturesParameters &cpy)
		{
			WindowSize = cpy.WindowSize;
			descriptorLength = cpy.descriptorLength;
			K = cpy.K;
		}

		double WindowSize;
		int K;
		int descriptorLength;
	};



	class DuneDirectionFeatureExtractor : public BaseFeatureExtractor
	{
	public:
		DuneDirectionFeatureExtractor()
		{

		}

		DuneDirectionFeatureExtractor(const DirectionalFeaturesParameters &params)
		{
			parameters = params;
		}

		DuneDirectionFeatureExtractor(const DuneDirectionFeatureExtractor &cpy)
		{
			parameters = cpy.parameters;
		}
		~DuneDirectionFeatureExtractor() {}

		void Process(const cv::Mat &img, std::vector<cv::KeyPoint> &points, cv::Mat &descriptor, const cv::Mat &scaleMat = cv::Mat()) const
		{
			cv::Mat dx, dy;
			cv::Sobel(img, dx, CV_64F, 1, 0, parameters.K);
			cv::Sobel(img, dy, CV_64F, 0, 1, parameters.K);

			descriptor = cv::Mat(points.size(), parameters.descriptorLength, CV_32F);

			for (size_t i = 0; i < points.size(); ++i)
			{
				double angle = std::atan2(dy.at<double>(points[i].pt), dx.at<double>(points[i].pt));

				points[i].angle = angle;

				cv::Mat d = getDescriptorAt(img, points[i]);
				//descriptor.row(i) = d;
				for (int j = 0; j < d.cols; ++j)
				{
					float val = d.at<float>(1, j);
					descriptor.at<float>(i, j) = val;
				}
			}
		}

	private:
		DirectionalFeaturesParameters parameters;

		cv::Mat getDescriptorAt(const cv::Mat &img, const cv::KeyPoint &point) const
		{
			cv::Vec2f d1, d2;
			d1[0] = std::cos(point.angle);
			d1[1] = std::sin(point.angle);
			d2[0] = std::cos(point.angle + 3.1416);
			d2[1] = std::sin(point.angle + 3.1416);

			cv::Point2f p1, p2;
			p1 = point.pt;
			p2 = point.pt;
			float sum = (float)img.at<uchar>(point.pt);
			float numPoints = 1;
			std::list<float> pixelVals;
			pixelVals.push_back(sum);
			for (int i = 0; i < parameters.K; i += 2)
			{
				movePoint(p1, d1, img.rows, img.cols);
				movePoint(p2, d2, img.rows, img.cols);
				float v1 = (float)img.at<uchar>(p1);
				float v2 = (float)img.at<uchar>(p2);
				sum += v1 + v2;
				numPoints += 2.0f;
				pixelVals.push_back(v1);
				pixelVals.push_front(v2);
			}

			float peakDiff = -1.0f * FLT_MAX;
			float previousAverage = sum / numPoints, average;
			bool stop = false;
			do
			{
				movePoint(p1, d1, img.rows, img.cols);
				movePoint(p2, d2, img.rows, img.cols);
				float v1 = (float)img.at<uchar>(p1);
				float v2 = (float)img.at<uchar>(p2);
				sum += v1 + v2;
				numPoints += 2.0f;
				average = sum / numPoints;
				float nDiff = average - previousAverage;
				if (nDiff > peakDiff) 
				{
					pixelVals.push_back(v1);
					pixelVals.push_front(v2);
					previousAverage = average;
					peakDiff = nDiff;
				}
				else
				{
					stop = true;
				}
			} while (!stop);
			
			return listToNormDescriptor(pixelVals);

		}

		cv::Mat listToNormDescriptor(std::list<float> &values) const
		{
			cv::Mat valMat(1, values.size(), CV_32F);
			int index = 0;
			for (std::list<float>::iterator it = values.begin(); it != values.end(); it++)
			{
				valMat.at<float>(0, index) = *it;
				index++;
			}
			
			cv::Mat normMat(1, parameters.descriptorLength, CV_32F);

			cv::resize(valMat, normMat, cv::Size(1, parameters.descriptorLength), 0.0, 0.0, cv::INTER_CUBIC);
			//cv::normalize(normMat, normMat, -1.0, 1.0, cv::NORM_MINMAX);
			return normMat;
		}

		void movePoint(cv::Point2f &pt, const cv::Vec2f &direction, int rows, int cols) const
		{
			pt.x += direction[0];
			pt.y += direction[1];
			if (pt.x >= cols)
				pt.x = cols - 1;
			if (pt.x < 0)
				pt.x = 0;
			if (pt.y >= rows)
				pt.y = rows - 1;
			if (pt.y < 0)
				pt.y = 0;

		}

	};
}