#ifndef _DUNE_DRAW_H_
#define _DUNE_DRAW_H_

#include "OpenCVHeaders.h"
#include "DuneSegment.h"

namespace dune
{
	namespace draw
	{
		static inline void DrawDuneSegment(cv::Mat &colorImage,
			const DuneSegment &segment,
			cv::Scalar color,
			bool drawLine = false,
			cv::Scalar lineColor=cv::Scalar(0,255,0),
			int lineThickness=1)
		{
			cv::Vec3b colorVal;
			colorVal[0] = color[0];
			colorVal[1] = color[1];
			colorVal[2] = color[2];
			std::vector<DuneSegmentData> segData = segment.GetSegmentData();
			for (size_t j = 0; j < segData.size(); ++j)
			{
				cv::Point p = segData[j].Position;
				colorImage.at<cv::Vec3b>(p) = colorVal;
			}

			if (drawLine)
			{
				cv::Vec4d line = segment.GetLine();
				cv::Point center(line[2], line[3]);
				double length = (double)segment.GetLength() / 2.0;
				cv::Point e1, e2;
				cv::Vec2d colinear;
				colinear[0] = line[0];
				colinear[1] = line[1];
				e1.x = center.x + colinear[0] * length;
				e1.y = center.y + colinear[1] * length;
				e2.x = center.x - colinear[0] * length;
				e2.y = center.y - colinear[1] * length;

				cv::line(colorImage, e1, e2, lineColor, lineThickness);
				cv::circle(colorImage, center, lineThickness, lineColor, lineThickness);
			}
		}


	}
}

#endif