#ifndef _DFT_IMAGE_ENHANCEMENT_H_
#define _DFT_IMAGE_ENHANCEMENT_H_

#include "DiscreteFourierTransform.h"
namespace dune
{
	namespace imgproc
	{
		class DFTImageEnhancement
		{
		public:
			DFTImageEnhancement(){}
			~DFTImageEnhancement(){}

			void Process(const cv::Mat &input, cv::Mat &output)
			{
				computeScaleImages(input);

				output = input.clone();
			}


		private:
			int _dSize = 11;
			int _numScales = 8;
			double r = 0.5;
			std::vector<cv::Mat> _scaleImages;
			DiscreteFourierTransform _dft;

			void computeAutoThreshold(const cv::Mat &input, cv::Mat &output)
			{
				cv::equalizeHist(input, output);
				cv::Scalar mean, stddev;
				cv::meanStdDev(output, mean, stddev);

				cv::Mat threshold;
				cv::threshold(output, output, mean[0] + r*stddev[0], 255, CV_THRESH_BINARY);
				cv::medianBlur(output, output, 11);
			}

			void computeScaleImages(const cv::Mat &input)
			{
				cv::Mat spectrum;
				_dft.Forward(input, spectrum);

				spectrum.convertTo(spectrum, CV_32F);

				cv::Scalar mean, stddev;
				cv::meanStdDev(spectrum, mean, stddev);

				_scaleImages.resize(_numScales);
				double tIncr = 8.0 / (double)(_numScales - 1);
				double tr = -2.0;
				double threshold = mean[0] + tr*stddev[0];
				for (int i = 0; i < _numScales; ++i)
				{
					threshold = mean[0] + tr*stddev[0];
					cv::Mat filter;
					cv::threshold(spectrum, filter, threshold, 1.0, cv::THRESH_BINARY_INV);
					cv::GaussianBlur(filter, filter, cv::Size(5, 5), 1.5, 1.5);
					tr += tIncr;

					_dft.MaskFilter(filter, _scaleImages[i]);

					cv::imshow("filter", filter);
					cv::imshow("scaleImage", _scaleImages[i]);
					cv::waitKey(0);
				}
			}

			//double getLocalGradientDirection(const cv::Mat &dx,
			//	const cv::Mat &dy,
			//	cv::Point anchor,
			//	int window,
			//	double &magnitude)
			//{
			//	double dxVal = 0.0;
			//	double dyVal = 0.0;
			//	for (int x = anchor.x - window / 2; x < anchor.x + window / 2; ++x)
			//	{
			//		if (x < 0 || x >= dx.cols)
			//			continue;

			//		for (int y = anchor.y - window / 2; y < anchor.y + window / 2; ++y)
			//		{
			//			if (y < 0 || y >= dx.rows)
			//				continue;
			//			dxVal += dx.at<double>(y, x) / 10000.0;
			//			dyVal += dy.at<double>(y, x) / 10000.0;
			//		}
			//	}

			//	magnitude = std::sqrt(dxVal*dxVal + dyVal*dyVal);
			//	return std::atan2(dyVal, dxVal);
			//}

			//double getValueAt(int x, int y, double gradiantDir, double gradientMag)
			//{
			//	if (gradiantDir < 0)
			//		gradiantDir += 3.1416;

			//	int i = 0, j = -1;
			//	double gradiantDirR = gradiantDir;
			//	while (i < _numDirections && gradiantDir >= _directions[i])
			//	{
			//		i++;
			//		j++;
			//	}
			//	if (i == _numDirections)
			//	{
			//		i = 0;
			//		gradiantDirR = 0.0;
			//	}


			//	double angleIncr = 3.1416 / (double)_numDirections;
			//	double l = std::fabs(gradiantDir - _directions[j]) / angleIncr;
			//	double r = std::fabs(gradiantDirR - _directions[i]) / angleIncr;

			//	double v1 = _directionImages[j].at<double>(y, x)*r;
			//	double v2 = _directionImages[i].at<double>(y, x)*l;

			//	return (v1 + v2);
			//}

			//void computeAngularScaleImage(cv::Mat &output)
			//{
			//	output = cv::Mat(_directionImages[0].rows, _directionImages[0].cols, CV_64F);
			//	for (int y = 0; y < _directionImages[0].rows; ++y)
			//	{
			//		for (int x = 0; x < _directionImages[0].cols; ++x)
			//		{
			//			std::vector<double> diff(_numDirections);

			//			/*diff[0] = std::fabs(_directionImages[0].at<double>(y, x) -
			//			_directionImages[_numDirections - 1].at<double>(y, x));
			//			for (int i = 1; i < _numDirections; ++i)
			//			{
			//			diff[i] = std::fabs(_directionImages[i].at<double>(y, x) -
			//			_directionImages[i - 1].at<double>(y, x));
			//			}*/

			//			for (int i = 0; i < _numDirections; ++i)
			//			{
			//				diff[i] = _directionImages[i].at<double>(y, x);
			//			}

			//			/*double max = DBL_MIN;
			//			double index = 0.0;
			//			for (int i = 1; i < _numDirections - 1; ++i)
			//			{
			//			if (diff[i] >= diff[i - 1] && diff[i] >= diff[i + 1] && diff[i] >= max)
			//			{
			//			max = diff[i];
			//			index = (double)i + (diff[i + 1] - diff[i - 1]) / (diff[i] + diff[i + 1] + diff[i - 1]);
			//			}
			//			}*/

			//			double min = DBL_MAX;
			//			double index = 0.0;
			//			for (int i = 1; i < _numDirections - 1; ++i)
			//			{
			//				if (diff[i] <= diff[i - 1] && diff[i] >= diff[i + 1] && diff[i] <= min)
			//				{
			//					min = diff[i];
			//					index = (double)i + (diff[i + 1] - diff[i - 1]) / (diff[i] + diff[i + 1] + diff[i - 1]);
			//				}
			//			}

			//			int left = std::floor(index);
			//			if (left < 0)
			//				left = _numDirections - 1;
			//			int right = left + 1;
			//			if (right >= _numDirections)
			//				right = 0.0;
			//			double l = index - (double)left;
			//			double r = 1.0 - l;

			//			output.at<double>(y, x) = _directionImages[right].at<double>(y, x)*r + _directionImages[left].at<double>(y, x)*l;

			//		}
			//	}

			//	cv::normalize(output, output, 0.0, 1.0, cv::NORM_MINMAX);
			//}


		};
	}

}


#endif
