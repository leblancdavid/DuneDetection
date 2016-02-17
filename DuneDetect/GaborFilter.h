#ifndef _GABOR_FILTER_H_
#define _GABOR_FILTER_H_

#include "OpenCVHeaders.h"

namespace dune
{
	namespace imgproc
	{
		class GaborFilter
		{
		public:
			GaborFilter(){}
			~GaborFilter(){}

			cv::Mat GetFrequencySpaceFilter(int maskHeight, int maskWidth, double dx, double dy, double angle=0.0)
			{
				cv::Mat mask(maskHeight, maskWidth, CV_64F);
				double du = 1.0 / 6.2832 * dx;
				double dv = 1.0 / 6.2832 * dy;
				double u0 = 2.0*du;

				for (int y = 0; y < maskHeight; ++y)
				{
					for (int x = 0; x < maskWidth; ++x)
					{
						cv::Point2d uv = Rotate(cv::Point2d((double)(x - maskWidth / 2),
							(double)(y - maskHeight / 2)), -1.0 * angle);
						double u = std::fabs(uv.x);
						double v = std::fabs(uv.y);
						double exp = -0.5 * ((u - u0)*(u - u0) / du / du + v*v / dv / dv);
						double value = 6.2832 * std::exp(exp);
						mask.at<double>(y, x) = value;
					}
				}

				cv::normalize(mask, mask, 0.0, 1.0, cv::NORM_MINMAX);

				return mask;
			}

		private:

			cv::Point2d Rotate(cv::Point2d pt, double angle)
			{
				double s = std::sin(angle);
				double c = std::cos(angle);

				// rotate point
				return cv::Point2d(pt.x * c - pt.y * s, pt.x * s + pt.y * c);
			}

		};
	}

}


#endif
