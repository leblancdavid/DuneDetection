#pragma once

#include "BaseFeatureClassifier.h"
#include "BaseFeatureExtractor.h"
#include "GaussianScalePyramid.h"
#include "DuneSegment.h"

namespace duneML
{
	class MLDuneFiltering
	{
	public:
		MLDuneFiltering(){ minSegmentLength = 15; }
		~MLDuneFiltering(){}
		MLDuneFiltering(const MLDuneFiltering &cpy)
		{
			classifier = cpy.classifier;
			detector = cpy.detector;
			minSegmentLength = 15;
		}
		
		MLDuneFiltering(BaseFeatureClassifier* fc, BaseFeatureExtractor* fe)
		{
			SetFeatureClassifier(fc);
			SetFeatureDetector(fe);
			minSegmentLength = 15;
		}

		void SetFeatureClassifier(BaseFeatureClassifier* fc)
		{
			classifier = fc;
		}

		void SetFeatureDetector(BaseFeatureExtractor* fe)
		{
			detector = fe;
		}

		void SetMinSegmentLength(int segLen)
		{
			minSegmentLength = segLen;
		}

		void SetK(int k)
		{
			K = k;
		}

		void SetThreshold(double t)
		{
			threshold = t;
		}

		std::vector<dune::DuneSegment> Filter(const cv::Mat &image, const std::vector<dune::DuneSegment> &segments)
		{
			GaussianScalePyramid gsp;
			//////LaplacianScalePyramid lsp;
			gsp.Process(image);
			cv::Mat scaleMap = gsp.GetScaleMap();

			std::vector<dune::DuneSegment> output;
			for (int i = 0; i < segments.size(); ++i)
			{
				std::vector<dune::DuneSegmentData> points = segments[i].GetSegmentData();
				std::vector<double> responses(points.size());
				std::vector<cv::KeyPoint> keypoints;
				for (int j = 0; j < points.size(); ++j)
				{
					keypoints.push_back(cv::KeyPoint(cv::Point2f(points[j].Position.x, points[j].Position.y), 0.0));
				}

				cv::Mat descriptors;
				detector->Process(image, keypoints, descriptors, scaleMap);

				for (int row = 0; row < descriptors.rows; ++row)
				{
					double val = classifier->Predict(descriptors.row(row));
					responses[row] = val;
				}

				std::vector<dune::DuneSegment> filteredSegments = FilterByResponses(segments[i], responses);
				for (int j = 0; j < filteredSegments.size(); ++j)
				{
					output.push_back(filteredSegments[j]);
				}
			}

			return output;
		}

		std::vector<dune::DuneSegment> FilterByResponse(const cv::Mat &image, const std::vector<dune::DuneSegment> &segments, double domOrientation)
		{
			cv::Mat responseMap = computeResponseMap(image);

			std::vector<dune::DuneSegment> shifted = segments;
			//ShiftSegmentsByResponse2(responseMap, shifted, domOrientation);
			ShiftSegmentsByResponse(responseMap, shifted, domOrientation);

			std::vector<dune::DuneSegment> output;
			for (int i = 0; i < shifted.size(); ++i)
			{
				//output.push_back(shifted[i]);

				std::vector<dune::DuneSegmentData> points = shifted[i].GetSegmentData();
				std::vector<double> responses(points.size());

				for (int j = 0; j < responses.size(); ++j)
				{
					responses[j] = responseMap.at<double>(points[j].Position);
				}

				std::vector<dune::DuneSegment> filteredSegments = FilterByResponses(shifted[i], responses);

				for (int j = 0; j < filteredSegments.size(); ++j)
				{
					output.push_back(filteredSegments[j]);
				}
			}

			return output;
		}

		std::vector<dune::DuneSegment> RemoveOverlappingSegments(const cv::Mat &image, const std::vector<dune::DuneSegment> &segments, int distance)
		{
			cv::Mat segmentsMap(image.rows, image.cols, CV_32S);
			segmentsMap = cv::Scalar(0);

			std::vector<dune::DuneSegment> output;
			//labels
			for (int i = 0; i < segments.size(); ++i)
			{
				for (int j = 0; j < segments[i].GetLength(); ++j)
				{
					segmentsMap.at<int>(segments[i][j].Position) += i + 1;
				}
			}

			double ratio = 0.9;
			for (int i = 0; i < segments.size(); ++i)
			{
				int count = 0;
				int length = segments[i].GetLength();
				for (int j = 0; j < length; ++j)
				{
					if (overlaps(segmentsMap, segments[i][j].Position, i + 1, distance))
					{
						count++;
					}
				}

				double r = (double)count / (double)length;
				if (r < ratio)
				{
					output.push_back(dune::DuneSegment(segments[i]));
				}

			}

			return output;
		}

	private:
		BaseFeatureClassifier* classifier;
		BaseFeatureExtractor* detector;

		int minSegmentLength;
		int K = 9;
		double threshold = 0.25;

		bool overlaps(const cv::Mat &map, const cv::Point &center, int label, int distance)
		{
			for (int i = center.y - distance; i < center.y + distance; ++i)
			{
				for (int j = center.x - distance; j < center.x + distance; ++j)
				{
					if (i < 0 || i >= map.rows || j < 0 || j >= map.cols)
						continue;

					if (map.at<int>(i, j) != 0 && map.at<int>(i, j) != label)
						return true;
				}
			}
			return false;
		}




		std::vector<dune::DuneSegment> FilterByResponses(const dune::DuneSegment &input, const std::vector<double> responses)
		{
			std::vector<dune::DuneSegmentData> data = input.GetSegmentData();
			std::vector<dune::DuneSegment> output;
			std::vector<bool> labels;

			int kSize = minSegmentLength;
			cv::Mat gaussian = cv::getGaussianKernel(kSize, kSize / 5.0);
			if (responses.size() < kSize)
			{
				return output;
			}

			for (int i = 0; i < data.size(); ++i)
			{
				int count = 0;
				double r = 0.0;
				for (int j = 0; j < kSize; ++j)
				{
					int k = i - kSize / 2 + j;
					if (k < 0)
						k = data.size() + k;
					else if (k >= (int)data.size())
						k = j - 1;

					r += responses[k] * gaussian.at<double>(j, 0);
				}

				if (r >= threshold)
				{
					labels.push_back(true);
				}
				else
				{
					labels.push_back(false);
				}
			}

			std::vector<dune::DuneSegmentData> segments;
			

			int start = 0;
			while (labels[start] && start < labels.size() - 1)
				start++;

			int i = start + 1;
			if (i >= labels.size())
				i = 0;

			while (i != start)
			{
				while (!labels[i] && i != start)
				{
					i++;

					if (i >= labels.size())
						i = 0;
				}

				while (labels[i] && i != start)
				{
					segments.push_back(data[i]);
					i++;
					if (i >= labels.size())
						i = 0;
				}
				if (segments.size() > minSegmentLength)
				{
					output.push_back(dune::DuneSegment(segments));
				}
				segments.clear();
			}

			//wrap around for the contour
			return output;
		}

		cv::Mat computeResponseMap(const cv::Mat &image)
		{
			cv::Mat responseMap = cv::Mat(image.rows, image.cols, CV_64F);
			GaussianScalePyramid gsp;
			//LaplacianScalePyramid lsp;
			//gsp.Process(image);
			//cv::Mat scaleMap = gsp.GetScaleMap();
			for (int y = 0; y < image.rows; ++y)
			{
				std::vector<cv::KeyPoint> allPoints;
				cv::Mat allDescriptors;

				for (int x = 0; x < image.cols; ++x)
				{
					cv::KeyPoint keypoint;
					keypoint.pt = cv::Point(x, y);
					allPoints.push_back(keypoint);
				}

				detector->Process(image, allPoints, allDescriptors);
				for (size_t i = 0; i < allPoints.size(); ++i)
				{
					double val = (double)classifier->Predict(allDescriptors.row(i));
					//std::cout << val << std::endl;
					responseMap.at<double>(allPoints[i].pt) = val;
				}

				
			}

			cv::normalize(responseMap, responseMap, -1.0, 1.0, cv::NORM_MINMAX);

			cv::imshow("responseMap", responseMap);
			cv::waitKey(30);

			return responseMap;
		}

		void ShiftSegmentsByResponse2(const cv::Mat& responseMap, std::vector<dune::DuneSegment> &segments, double domOrientation)
		{
			double searchDir = domOrientation - 3.1416;
			double x_incr = std::cos(searchDir);
			double y_incr = std::sin(searchDir);

			for (size_t i = 0; i < segments.size(); ++i)
			{
				ShiftSegmentToOptimalResponse(responseMap, segments[i], K, searchDir);
				segments[i].GaussianSmooth(K);
			}
		}

		void ShiftSegmentToOptimalResponse(const cv::Mat& responseMap, dune::DuneSegment &segment, int kSize, double dir)
		{
			double searchDir = dir - 3.1416;
			double x_incr = std::cos(searchDir);
			double y_incr = std::sin(searchDir);

			double optimalMag = 0.0;
			int peakK = 0;
			//Find the optimal shift
			for (int k = 0; k < kSize; ++k)
			{
				double totalMag = 0.0;
				double magCount = 0.0;
				int length = segment.GetLength();
				for (int i = 0; i < length; ++i)
				{
					double x = k * x_incr + segment[i].Position.x;
					double y = k * y_incr + segment[i].Position.y;
					if (x < 0 || x >= responseMap.cols ||
						y < 0 || y >= responseMap.rows)
						continue;

					magCount++;
					totalMag += responseMap.at<double>(y, x);
				}
				totalMag /= magCount;
				if (totalMag > optimalMag)
				{
					peakK = k;
					optimalMag = totalMag;
				}
			}

			//apply the shift
			for (int i = 0; i < segment.GetLength(); ++i)
			{
				segment[i].Position.x = std::ceil((double)peakK * x_incr + (double)segment[i].Position.x - 0.5);
				segment[i].Position.y = std::ceil((double)peakK * y_incr + (double)segment[i].Position.y - 0.5);

				if (segment[i].Position.x < 0.0)
					segment[i].Position.x = 0.0;
				else if (segment[i].Position.x >= responseMap.cols)
					segment[i].Position.x = responseMap.cols - 1;
				if (segment[i].Position.y < 0.0)
					segment[i].Position.y = 0.0;
				else if (segment[i].Position.y >= responseMap.rows)
					segment[i].Position.y = responseMap.rows - 1;
			}
		}












		void ShiftSegmentsByResponse(const cv::Mat& responseMap, std::vector<dune::DuneSegment> &segments, double domOrientation)
		{
			double searchDir = domOrientation - 3.1416;
			double x_incr = std::cos(searchDir);
			double y_incr = std::sin(searchDir);

			for (size_t i = 0; i < segments.size(); ++i)
			{
				std::vector<cv::Vec2d> shiftValues(segments[i].GetLength());

				for (int j = 0; j < segments[i].GetLength(); ++j)
				{
					Get2dShiftResponse(responseMap, 
						segments[i][j].Position.x,
						segments[i][j].Position.y,
						searchDir,
						shiftValues[j]);

				}
				SmoothShifts(shiftValues, K * 2);
				for (int j = 0; j < segments[i].GetLength(); ++j)
				{
					segments[i][j].Position.x += shiftValues[j][0];
					segments[i][j].Position.y += shiftValues[j][1];

					if (segments[i][j].Position.x < 0)
						segments[i][j].Position.x = 0;
					else if (segments[i][j].Position.x >= responseMap.cols)
						segments[i][j].Position.x = responseMap.cols - 1;

					if (segments[i][j].Position.y < 0)
						segments[i][j].Position.y = 0;
					else if (segments[i][j].Position.y >= responseMap.rows)
						segments[i][j].Position.y = responseMap.rows - 1;
				}

				segments[i].GaussianSmooth(K * 2);
			}
		}

		void SmoothShifts(std::vector<cv::Vec2d> &shift, int kSize)
		{
			if (shift.size() < kSize)
				return;

			std::vector<cv::Vec2d> processed(shift.size());
			cv::Mat gausK = cv::getGaussianKernel(kSize, kSize / 5.0);

			for (int i = 0; i < shift.size(); ++i)
			{
				int count = 0;
				double x = 0.0;
				double y = 0.0;
				for (int j = 0; j < kSize; ++j)
				{
					int k = i - kSize / 2 + j;
					if (k < 0)
						k = shift.size() + k;
					else if (k >= (int)shift.size())
						k = j - 1;

					x += shift[k][0] * gausK.at<double>(j, 0);
					y += shift[k][1] * gausK.at<double>(j, 0);
				}

				processed[i][0] = x;
				processed[i][1] = y;
			}

			shift = processed;
		}

		bool Get2dShiftResponse(const cv::Mat &responseMap, double x, double y, double dir, cv::Vec2d &shift)
		{
			bool found = false;
			double x_incr = std::cos(dir);
			double y_incr = std::sin(dir);

			shift = cv::Vec2d(0.0, 0.0);
			double maxMag = -999999.9;
			for (int k = -1*K; k < K; ++k)
			{
				double xshift = x + k * x_incr;
				double yshift = y + k * y_incr;

				if (xshift < 0 || xshift >= responseMap.cols ||
					yshift < 0 || yshift >= responseMap.rows)
					continue;

				double mag = responseMap.at<double>(yshift, xshift);
				if (mag > maxMag)
				{
					maxMag = mag;
					shift[0] = k * x_incr;
					shift[1] = k * y_incr;
					found = true;
				}
			}

			return found;
		}
	};
}