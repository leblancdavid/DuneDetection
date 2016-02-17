#ifndef _BASE_EDGE_IMAGE_PROCESSOR_H_
#define _BASE_EDGE_IMAGE_PROCESSOR_H_

#include "BaseImageProcessor.h"

namespace dune
{
	enum DominantOrientationMethod { HOG, K_MEANS };

	const int GRADIENT_MAT_DX_INDEX = 0;
	const int GRADIENT_MAT_DY_INDEX = 1;
	const int GRADIENT_MAT_MAGNITUDE_INDEX = 2;
	const int GRADIENT_MAT_DIRECTION_INDEX = 3;

	class ImageGradientData
	{
	public:
		ImageGradientData() {}
		virtual ~ImageGradientData()
		{
			Gradient = cv::Mat();
		}
		ImageGradientData(const ImageGradientData &cpy)
		{
			Gradient = cpy.Gradient.clone();
			Mean = cpy.Mean;
			StdDev = cpy.StdDev;
		}

		cv::Mat toImage()
		{
			cv::Mat image;
			cv::normalize(Gradient, image, 0, 255, cv::NORM_MINMAX);
			image.convertTo(image, CV_8UC4);
			return image;
		}
		cv::Mat toImage(int prop, const cv::Mat &scale = cv::Mat())
		{
			if (prop < 0 || prop > GRADIENT_MAT_DIRECTION_INDEX)
			{
				return toImage();
			}
			else if (!scale.empty() && prop == GRADIENT_MAT_DIRECTION_INDEX)
			{
				return toScaleDirectionImage(scale);
			}
			else if (!scale.empty() && prop == GRADIENT_MAT_MAGNITUDE_INDEX)
			{
				return toScaleMagnitudeImage(scale);
			}
			else
			{
				cv::Mat image;
				std::vector<cv::Mat> channels;
				cv::split(Gradient, channels);
				//cv::normalize(channels[prop], image, 0, 255, cv::NORM_MINMAX);
				//image.convertTo(image, CV_8UC1);
				return channels[prop];
			}
		}

		double getMagnitudeAt(int x, int y)
		{
			return Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_MAGNITUDE_INDEX];
		}
		double getXDerivAt(int x, int y)
		{
			return Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DX_INDEX];
		}
		double getYDerivAt(int x, int y)
		{
			return Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DY_INDEX];
		}
		double getAngleAt(int x, int y)
		{
			return Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DIRECTION_INDEX];
		}

		cv::Mat Gradient;
		cv::Scalar Mean;
		cv::Scalar StdDev;

	private:
		cv::Mat toScaleDirectionImage(const cv::Mat &scale)
		{
			std::vector<cv::Mat> channels;
			cv::split(Gradient, channels);
			cv::Mat normDx, normDy, inDx, inDy, output(scale.rows, scale.cols, CV_64F);
			cv::normalize(channels[GRADIENT_MAT_DX_INDEX], normDx, 0.0, 1.0, cv::NORM_MINMAX);
			cv::normalize(channels[GRADIENT_MAT_DY_INDEX], normDy, 0.0, 1.0, cv::NORM_MINMAX);
			cv::integral(normDx, inDx);
			cv::integral(normDy, inDy);

			for (int x = 0; x < scale.cols; ++x)
			{
				for (int y = 0; y < scale.rows; ++y)
				{
					int radius = 2;
					double dx = calcSumAt(x, y, inDx, radius);
					double dy = calcSumAt(x, y, inDy, radius);

					output.at<double>(y, x) = std::atan2(dy, dx);
				}
			}

			return output;
		}

		cv::Mat toScaleMagnitudeImage(const cv::Mat &scale)
		{
			//TODO:
			return cv::Mat();
		}

		double calcSumAt(int x, int y, const cv::Mat &integralImg, int radius)
		{
			int left = x - radius;
			int right = x + radius;
			int top = y - radius;
			int bottom = y + radius;

			if (left < 0)
				left = 0;
			if (right >= integralImg.cols)
				right = integralImg.cols - 1;
			if (top < 0)
				top = 0;
			if (bottom >= integralImg.rows)
				bottom = integralImg.rows - 1;

			int width = right - left;
			int height = bottom - top;
			double area = width * height;

			cv::Point A, B, C, D;
			A.x = left, A.y = top;
			B.x = right, B.y = top;
			C.x = left, C.y = bottom;
			D.x = right, D.y = bottom;

			return (integralImg.at<double>(D) -integralImg.at<double>(C) -integralImg.at<double>(B) +integralImg.at<double>(A))/area;
		}

	};

	class BasedEdgeImageProcessor : public BaseImageProcessor
	{
	public:
		BasedEdgeImageProcessor() 
		{
			lastComputedDominantOrientation = 0.0;
			dominantOrientationComputed = false;
		};

		~BasedEdgeImageProcessor() { };

		BasedEdgeImageProcessor(const BasedEdgeImageProcessor &cpy) 
		{
			lastComputedDominantOrientation = cpy.lastComputedDominantOrientation;
			dominantOrientationComputed = cpy.dominantOrientationComputed;
		};

		bool IsDominantOrientationComputed()
		{
			return dominantOrientationComputed;
		}

		double GetDominantOrientation()
		{
			return lastComputedDominantOrientation;
		}

		virtual void Process(const cv::Mat &inputImg, cv::Mat &outputImg) = 0;

		void ComputeGradient(const cv::Mat &inputImg, int K)
		{
			BaseData.Gradient = cv::Mat(inputImg.rows, inputImg.cols, CV_64FC4);
			cv::Mat dx, dy;
			cv::Sobel(inputImg, dx, CV_64F, 1, 0, K);
			cv::Sobel(inputImg, dy, CV_64F, 0, 1, K);

			for (int x = 0; x < inputImg.cols; ++x)
			{
				for (int y = 0; y < inputImg.rows; ++y)
				{
					cv::Vec4d g;
					g[GRADIENT_MAT_DX_INDEX] = dx.at<double>(y, x);
					g[GRADIENT_MAT_DY_INDEX] = dy.at<double>(y, x);
					g[GRADIENT_MAT_MAGNITUDE_INDEX] = std::sqrt(g[GRADIENT_MAT_DX_INDEX] * g[GRADIENT_MAT_DX_INDEX] +
																g[GRADIENT_MAT_DY_INDEX] * g[GRADIENT_MAT_DY_INDEX]);
					g[GRADIENT_MAT_DIRECTION_INDEX] = std::atan2(g[GRADIENT_MAT_DY_INDEX], g[GRADIENT_MAT_DX_INDEX]);
					BaseData.Gradient.at<cv::Vec4d>(y, x) = g;
				}
			}

			cv::meanStdDev(BaseData.Gradient, BaseData.Mean, BaseData.StdDev);
		}

		//Finds the dominant orientation using HoG method
		double ComputeDominantOrientation(DominantOrientationMethod method, int bins)
		{
			std::vector<cv::Point> points;
			for (int x = 0; x < BaseData.Gradient.cols; ++x)
			{
				for (int y = 0; y < BaseData.Gradient.rows; ++y)
				{
					points.push_back(cv::Point(x, y));
				}
			}

			if (method == HOG)
			{
				lastComputedDominantOrientation = FindDominantOrientationUsingHoG(points, bins);
				dominantOrientationComputed = true;
				return lastComputedDominantOrientation;
			}
			else if (method == K_MEANS)
			{
				lastComputedDominantOrientation = FindDominantOrientationUsingKMeans(points, bins);
				dominantOrientationComputed = true;
				return lastComputedDominantOrientation;
			}

			
		}

		double FindDominantOrientationUsingHoG(const std::vector<cv::Point> &points, int bins)
		{
			double avgOrientation = BaseData.Mean[GRADIENT_MAT_DIRECTION_INDEX];

			//Init the HoG Bins
			double increments = 2.0*3.1416 / (double)bins;
			std::vector<double> HoG(bins);
			std::vector<cv::Point2d> HoG2D(bins);
			for (size_t i = 0; i < HoG.size(); ++i)
			{
				HoG2D[i].x = 0.0;
				HoG2D[i].y = 0.0;
			}

			//Compute the normalized orientation.
			for (size_t i = 0; i < points.size(); ++i)
			{
				double dx = BaseData.Gradient.at<cv::Vec4d>(points[i])[GRADIENT_MAT_DX_INDEX] - BaseData.Mean[GRADIENT_MAT_DX_INDEX];
				double dy = BaseData.Gradient.at<cv::Vec4d>(points[i])[GRADIENT_MAT_DY_INDEX] - BaseData.Mean[GRADIENT_MAT_DY_INDEX];

				double orientation = std::atan2(dy, dx) - avgOrientation;
				while (orientation < 0)
					orientation += 2.0*3.1416;

				int bin = std::ceil((orientation / increments) - 0.5);
				if (bin >= bins)
					bin = 0;

				HoG2D[bin].x += dx / 1000.0;
				HoG2D[bin].y += dy / 1000.0;
			}

			int index = 0;
			for (size_t i = 0; i < HoG.size(); ++i)
			{
				double dx = HoG2D[i].x / 1000.0;
				double dy = HoG2D[i].y / 1000.0;
				HoG[i] = std::sqrt(dx*dx + dy*dy);
			}

			return calcDominantOrientationFromHoG(HoG, HoG2D);
		}

		double FindDominantOrientationUsingKMeans(const std::vector<cv::Point> &points, int bins)
		{
			std::vector<cv::Point2f> gradients;

			double t = BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX] + 4.0 * BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];
			for (size_t i = 0; i < points.size(); ++i)
			{
				if (BaseData.Gradient.at<cv::Vec4d>(points[i].y, points[i].x)[GRADIENT_MAT_MAGNITUDE_INDEX] > t)
				{
					float dx = BaseData.Gradient.at<cv::Vec4d>(points[i].y, points[i].x)[GRADIENT_MAT_DX_INDEX] / BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
					float dy = BaseData.Gradient.at<cv::Vec4d>(points[i].y, points[i].x)[GRADIENT_MAT_DY_INDEX] / BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];

					gradients.push_back(cv::Point2f(dx, dy));
				}
				
			}

			int clusters = bins;
			std::vector<int> labels;
			cv::Mat centers;
			cv::kmeans(gradients, clusters, labels, cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 0.1), 5, cv::KMEANS_PP_CENTERS, centers);

			double domOrientation;
			double maxMag = 0;
			int index = 0;
			for (int i = 0; i < centers.rows; ++i)
			{
				double m = centers.at<float>(i, 0)*centers.at<float>(i, 0) + centers.at<float>(i, 1)*centers.at<float>(i, 1);
				if (m > maxMag)
				{
					maxMag = m;
					domOrientation = std::atan2(centers.at<float>(i, 1), centers.at<float>(i, 0));
					index = i;
				}
			}

			return domOrientation;
		}

		//The gradient matrix is a 4-channel matrix where 
		//C1: dx, C2: dy, C3: magnitude, C4: direction 
		ImageGradientData BaseData;

	private:

		double lastComputedDominantOrientation;
		bool dominantOrientationComputed;

		double calcDominantOrientationFromHoG(const std::vector<double> &histogram, const std::vector<cv::Point2d> &gradients)
		{
			double dominantDir;
			double dominantMag = 0;
			for (int i = 0; i < histogram.size(); ++i)
			{
				int p = i - 1;
				int n = i + 1;
				if (p < 0)
					p = histogram.size() - 1;
				if (n >= histogram.size())
					n = 0;

				if (histogram[i] > histogram[p] && histogram[i] > histogram[n] && histogram[i] > dominantMag)
				{
					dominantMag = histogram[i];

					double dx = gradients[i].x;
					double dy = gradients[i].y;

					dominantDir = std::atan2(dy, dx);
				}
			}

			return dominantDir;
		}

	};



}

#endif