#ifndef _DUNE_SEGMENT_DETECTOR_H_
#define _DUNE_SEGMENT_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "AdaptiveImageProcessor.h"

namespace dune
{

struct DuneSegmentData
{
	cv::Point Position;
	double Orientation;
};

class DuneSegment
{
public:
	DuneSegment() { }
	~DuneSegment() { }
	DuneSegment(const DuneSegment &cpy)
	{
		Data = cpy.Data;
	}

	std::vector<DuneSegmentData> Data;
private:

};

class DuneSegmentDetectorParameters
{
public:
	DuneSegmentDetectorParameters()
	{
		MinContourLength = 50;
	}

	int MinContourLength;
};

class DuneSegmentDetector : public BaseDuneDetector<DuneSegment>
{
public:
	DuneSegmentDetector();
	DuneSegmentDetector(BaseImageProcessor* imgproc, const DuneSegmentDetectorParameters &params);
	~DuneSegmentDetector();

	std::vector<DuneSegment> Extract(const cv::Mat &img);

private:

	DuneSegmentDetectorParameters Parameters;
	std::vector<std::vector<cv::Point>> GetContours(const cv::Mat &img);
	std::vector<std::vector<cv::Point>> SplitContourSegments(const std::vector<cv::Point> &contour);
};

}
#endif