#ifndef _BASE_FEATURE_EXTRACTOR_H_
#define _BASE_FEATURE_EXTRACTOR_H_

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

namespace duneML
{
	class BaseFeatureExtractor
	{
	public:
		BaseFeatureExtractor() {}
		virtual ~BaseFeatureExtractor() {}

		virtual void Process(const cv::Mat &img, std::vector<cv::KeyPoint> &points, cv::Mat &descriptor, const cv::Mat &scaleMat = cv::Mat()) const = 0;

	protected:
		virtual double getDominantOrientation(const cv::Mat &dx, const cv::Mat &dy, const cv::Rect &region) const
		{
			cv::Rect adjustedRegion = region;
			if (adjustedRegion.x < 0)
				adjustedRegion.x = 0.0;
			else if (adjustedRegion.x >= dx.cols)
				adjustedRegion.x = dx.cols - 1;
			if (adjustedRegion.y < 0)
				adjustedRegion.y = 0.0;
			else if (adjustedRegion.y >= dx.rows)
				adjustedRegion.y = dx.rows - 1;

			if (adjustedRegion.x + adjustedRegion.width >= dx.cols)
				adjustedRegion.width = dx.cols - adjustedRegion.x - 1;
			if (adjustedRegion.y + adjustedRegion.height >= dx.rows)
				adjustedRegion.height = dx.rows - adjustedRegion.y - 1;

			cv::Mat dxRoi = dx(adjustedRegion);
			cv::Mat dyRoi = dy(adjustedRegion);

			double dxVal = 0.0;
			double dyVal = 0.0;
			for (int x = 0; x < dxRoi.cols; ++x)
			{
				for (int y = 0; y < dxRoi.rows; ++y)
				{
					dxVal += dx.at<double>(y, x) / 10000.0;
					dyVal += dy.at<double>(y, x) / 10000.0;
				}
			}

			return std::atan2(dyVal, dxVal);
		}

	private:
	};
}

#endif