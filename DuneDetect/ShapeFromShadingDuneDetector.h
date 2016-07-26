#pragma once

#include "ShapeFromShadingProcessor.h"

namespace dune
{
	class ShapeFromShadingDetectorParameters : public BaseDetectorParameters
	{
	public:
		ShapeFromShadingDetectorParameters()
		{
			MinSegmentLength = 100;
		}
		ShapeFromShadingDetectorParameters(const ShapeFromShadingDetectorParameters &cpy)
		{
			MinSegmentLength = cpy.MinSegmentLength;
		}
		ShapeFromShadingDetectorParameters(int pMinContourLength)
		{
			MinSegmentLength = pMinContourLength;
		}

		int MinSegmentLength;
	};

	class ShapeFromShadingDetector : public BaseDuneDetector
	{
	public:
		ShapeFromShadingDetector();
		ShapeFromShadingDetector(BaseImageProcessor* imgproc, ShapeFromShadingDetectorParameters *params);
		~ShapeFromShadingDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

		void SetParameters(BaseDetectorParameters *parameters)
		{
			Parameters = static_cast<ShapeFromShadingDetectorParameters*>(parameters);
			if (ImageProcess != NULL)
			{
				ImageProcess->SetParameters(Parameters->ImageProcessParameters);
			}
		}

	private:

		ShapeFromShadingDetectorParameters *Parameters;
	};




}