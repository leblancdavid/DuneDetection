#ifndef _DISCRETE_FOURIER_TRANSFORM_H_
#define _DISCRETE_FOURIER_TRANSFORM_H_

#include "OpenCVHeaders.h"

namespace dune
{
	namespace imgproc
	{
		class DiscreteFourierTransform
		{
		public:
			DiscreteFourierTransform() { }
			~DiscreteFourierTransform() { }
			DiscreteFourierTransform(const DiscreteFourierTransform &cpy)
			{
				ComplexDFT = cpy.ComplexDFT.clone();
			}

			void Forward(const cv::Mat &input, cv::Mat &output)
			{
				ComputeForwardDFT(input);
				output = GetDFTSpectrumImage();
			}

			void Inverse(cv::Mat &output)
			{
				ComputeInverseDFT(ComplexDFT, output);
			}

			void HighPassFilter(const cv::RotatedRect &filter, cv::Mat &output)
			{
				cv::Mat mask(ComplexDFT.rows, ComplexDFT.cols, CV_8UC1);
				mask = cv::Scalar(255);
				cv::RotatedRect f_mask = filter;
				DrawFilledEllipse(mask, f_mask, cv::Scalar(0));

				cv::imwrite("DFT_mask.jpg", mask);

				ShiftQuadrantsForDFT(mask, mask);

				/*f_mask.center.x = 0.0;
				f_mask.center.y = 0.0;
				DrawFilledEllipse(mask, f_mask, cv::Scalar(0));
				f_mask.center.x = ComplexDFT.cols;
				DrawFilledEllipse(mask, f_mask, cv::Scalar(0));
				f_mask.center.y = ComplexDFT.rows;
				DrawFilledEllipse(mask, f_mask, cv::Scalar(0));
				f_mask.center.x = 0.0;
				DrawFilledEllipse(mask, f_mask, cv::Scalar(0));*/

				cv::imshow("mask", mask);
				cv::waitKey(0);

				cv::Mat processedDFT = cv::Mat(ComplexDFT.size(), CV_64FC2);
				for (int x = 0; x < processedDFT.cols; ++x)
				{
					for (int y = 0; y < processedDFT.rows; ++y)
					{
						if (mask.at<uchar>(y, x))
						{
							processedDFT.at<cv::Vec2d>(y, x) = ComplexDFT.at<cv::Vec2d>(y, x);
						}
						else
						{
							processedDFT.at<cv::Vec2d>(y, x) = cv::Vec2d(0.0, 0.0);
						}
					}
				}

				ComputeInverseDFT(processedDFT, output);
			}

			cv::Mat GetDFTSpectrumImage()
			{
				cv::Mat planes[2], mag;
				cv::split(ComplexDFT, planes);
				cv::magnitude(planes[0], planes[1], mag);

				//add 1 so we can convert it to the logarithmic scale
				mag += cv::Scalar::all(1);
				cv::log(mag, mag);

				// crop the spectrum, if it has an odd number of rows or columns
				mag = mag(cv::Rect(0, 0, mag.cols & -2, mag.rows & -2));

				// rearrange the quadrants of Fourier image  so that the origin is at the image center
				ShiftQuadrantsForDFT(mag, mag);
				
				cv::normalize(mag, mag, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
				// viewable image form (float between values 0 and 1).

				return mag;
			}

		private:

			cv::Mat ComplexDFT;
			cv::Size SourceImageSize;
			void ComputeForwardDFT(const cv::Mat &input)
			{
				//This code was copied from the opencv example.
				//file:///E:/Projects/Thesis/DuneDetection/Papers/ToRead/Discrete%20Fourier%20Transform%20%E2%80%94%20OpenCV%202.4.11.0%20documentation.html
				cv::Mat padded;
				SourceImageSize = input.size();
				int m = cvGetOptimalDFTSize(input.rows);
				int n = cvGetOptimalDFTSize(input.cols);
				cv::copyMakeBorder(input, padded, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

				cv::Mat planes[] = { cv::Mat_<double>(padded), cv::Mat::zeros(padded.size(), CV_64F) };

				cv::merge(planes, 2, ComplexDFT);
				cv::dft(ComplexDFT, ComplexDFT);
			}

			void ComputeInverseDFT(const cv::Mat &input, cv::Mat &output)
			{
				cv::dft(input, output, cv::DFT_INVERSE);
				cv::Mat planes[2], mag;
				cv::split(output, planes);
				cv::magnitude(planes[0], planes[1], mag);

				//remove the border
				output = cv::Mat(mag, cv::Rect(0, 0, SourceImageSize.width, SourceImageSize.height));

				cv::normalize(output, output, 0, 1, CV_MINMAX);
			}

			void DrawFilledEllipse(cv::Mat &image, const cv::RotatedRect &ellipse, const cv::Scalar color)
			{
				std::vector<cv::Point> vertices;

				cv::ellipse2Poly(ellipse.center, cv::Size(ellipse.size.width/2, ellipse.size.height/2), ellipse.angle, ellipse.angle, ellipse.angle + 360, 1, vertices);
				//make sure the vertices are actually on the image
				for (size_t i = 0; i < vertices.size(); ++i)
				{
					if (vertices[i].x < 0)
						vertices[i].x = 0;
					else if (vertices[i].x >= image.cols)
						vertices[i].x = image.cols - 1;
					if (vertices[i].y < 0)
						vertices[i].y = 0;
					else if (vertices[i].y >= image.rows)
						vertices[i].y = image.rows - 1;

				}

				std::vector<std::vector<cv::Point>> polygons;
				polygons.push_back(vertices);
				cv::fillPoly(image, polygons, color);
			}

			void ShiftQuadrantsForDFT(const cv::Mat &input, cv::Mat &shifted)
			{
				shifted = input.clone();
				// rearrange the quadrants of Fourier image  so that the origin is at the image center
				int cx = shifted.cols / 2;
				int cy = shifted.rows / 2;

				cv::Mat q0(shifted, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
				cv::Mat q1(shifted, cv::Rect(cx, 0, cx, cy));  // Top-Right
				cv::Mat q2(shifted, cv::Rect(0, cy, cx, cy));  // Bottom-Left
				cv::Mat q3(shifted, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

				cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
				q0.copyTo(tmp);
				q3.copyTo(q0);
				tmp.copyTo(q3);

				q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
				q2.copyTo(q1);
				tmp.copyTo(q2);
			}

			


		};
	}

}


#endif