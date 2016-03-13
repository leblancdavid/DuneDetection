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

		std::vector<dune::DuneSegment> Filter(const cv::Mat &image, const std::vector<dune::DuneSegment> &segments)
		{
			//GaussianScalePyramid gsp;
			//////LaplacianScalePyramid lsp;
			//gsp.Process(image);
			//cv::Mat scaleMap = gsp.GetScaleMap();

			std::vector<dune::DuneSegment> output;
			for (int i = 0; i < segments.size(); ++i)
			{
				std::vector<dune::DuneSegmentData> points = segments[i].GetSegmentData();
				std::vector<float> responses(points.size());
				std::vector<cv::KeyPoint> keypoints;
				for (int j = 0; j < points.size(); ++j)
				{
					keypoints.push_back(cv::KeyPoint(cv::Point2f(points[j].Position.x, points[j].Position.y), 0.0));
				}

				cv::Mat descriptors;
				detector->Process(image, keypoints, descriptors/*, scaleMap*/);

				for (int row = 0; row < descriptors.rows; ++row)
				{
					float val = classifier->Predict(descriptors.row(row));
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
			ShiftSegmentsByResponse(responseMap, shifted, responseMap, domOrientation);

			std::vector<dune::DuneSegment> output;
			for (int i = 0; i < shifted.size(); ++i)
			{
				std::vector<dune::DuneSegmentData> points = shifted[i].GetSegmentData();
				std::vector<float> responses(points.size());

				for (int j = 0; j < responses.size(); ++j)
				{
					responses[j] = responseMap.at<float>(points[j].Position);
				}

				std::vector<dune::DuneSegment> filteredSegments = FilterByResponses(shifted[i], responses);
				for (int j = 0; j < filteredSegments.size(); ++j)
				{
					output.push_back(filteredSegments[j]);
				}
			}

			return output;
		}

	private:
		BaseFeatureClassifier* classifier;
		BaseFeatureExtractor* detector;

		int minSegmentLength;
		int K = 9;


		std::vector<dune::DuneSegment> FilterByResponses(const dune::DuneSegment &input, const std::vector<float> responses)
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

				if (r >= 0)
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
			gsp.Process(image);
			cv::Mat scaleMap = gsp.GetScaleMap();
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

				detector->Process(image, allPoints, allDescriptors/*, scaleMap*/);
				for (size_t i = 0; i < allPoints.size(); ++i)
				{
					double val = (double)classifier->Predict(allDescriptors.row(i));
					//std::cout << val << std::endl;
					responseMap.at<double>(allPoints[i].pt) = val;
				}
			}

			return responseMap;
		}

		void ShiftSegmentsByResponse(const cv::Mat& responseMap, std::vector<dune::DuneSegment> &segments, const cv::Mat &domMap, double domOrientation)
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