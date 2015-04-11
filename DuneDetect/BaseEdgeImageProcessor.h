#ifndef _BASE_EDGE_IMAGE_PROCESSOR_H_
#define _BASE_EDGE_IMAGE_PROCESSOR_H_

#include "BaseImageProcessor.h"

namespace dune
{
	enum DominantOrientationMethod { HOG };

	const int GRADIENT_MAT_DX_INDEX = 0;
	const int GRADIENT_MAT_DY_INDEX = 1;
	const int GRADIENT_MAT_MAGNITUDE_INDEX = 2;
	const int GRADIENT_MAT_DIRECTION_INDEX = 3;

	class ImageGradientData
	{
	public:
		ImageGradientData() {}
		~ImageGradientData() {}
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
		cv::Mat toImage(int prop)
		{
			if (prop < 0 || prop > GRADIENT_MAT_DIRECTION_INDEX)
			{
				return toImage();
			}
			else
			{
				cv::Mat image;
				std::vector<cv::Mat> channels;
				cv::split(Gradient, channels);
				cv::normalize(channels[prop], image, 0, 255, cv::NORM_MINMAX);
				image.convertTo(image, CV_8UC1);
				return image;
			}
		}

		cv::Mat Gradient;
		cv::Scalar Mean;
		cv::Scalar StdDev;
	};

	class BasedEdgeImageProcessor : public BaseImageProcessor
	{
	public:
		BasedEdgeImageProcessor() {};
		~BasedEdgeImageProcessor() {};
		BasedEdgeImageProcessor(const BasedEdgeImageProcessor &cpy) {};

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

		double FindDominantOrientation(DominantOrientationMethod method, int param1)
		{
			if (method = HOG)
				return FindDominantOrientationUsingHoG(param1);
			else
				return 0.0;

		}

		//The gradient matrix is a 4-channel matrix where 
		//C1: dx, C2: dy, C3: magnitude, C4: direction 
		ImageGradientData BaseData;

	protected:
	
		//Finds the dominant orientation using HoG method
		double FindDominantOrientationUsingHoG(int bins)
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
			for (int x = 0; x < BaseData.Gradient.cols; ++x)
			{
				for (int y = 0; y < BaseData.Gradient.rows; ++y)
				{
					double dx = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DX_INDEX] - BaseData.Mean[GRADIENT_MAT_DX_INDEX];
					double dy = BaseData.Gradient.at<cv::Vec4d>(y, x)[GRADIENT_MAT_DY_INDEX] - BaseData.Mean[GRADIENT_MAT_DY_INDEX];

					double orientation = std::atan2(dy, dx) - avgOrientation;
					while (orientation < 0)
						orientation += 2.0*3.1416;

					int bin = std::ceil((orientation / increments) - 0.5);
					if (bin >= bins)
						bin = 0;

					HoG2D[bin].x += dx / 1000.0;
					HoG2D[bin].y += dy / 1000.0;
				}
			}

			int index = 0;
			for (size_t i = 0; i < HoG.size(); ++i)
			{
				double dx = HoG2D[i].x / 1000.0;
				double dy = HoG2D[i].y / 1000.0;
				HoG[i] = std::sqrt(dx*dx + dy*dy);
			}

			double dominantDir;
			double dominantMag = 0;
			for (int i = 0; i < HoG.size(); ++i)
			{
				int p = i - 1;
				int n = i + 1;
				if (p < 0)
					p = HoG.size() - 1;
				if (n >= HoG.size())
					n = 0;

				if (HoG[i] > HoG[p] && HoG[i] > HoG[n] && HoG[i] > dominantMag)
				{
					dominantMag = HoG[i];

					double dx = HoG2D[i].x;
					double dy = HoG2D[i].y;

					dominantDir = std::atan2(dy, dx);
				}
			}

			return dominantDir;
		}

	private:

	};



}

#endif