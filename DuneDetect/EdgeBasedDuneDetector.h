#ifndef _EDGE_BASED_DUNE_DETECTOR_H_
#define _EDGE_BASED_DUNE_DETECTOR_H_

#include "OpenCVHeaders.h"
#include "BaseDuneDetector.h"
#include "EdgeDetectorImageProcessor.h"

namespace dune
{

	class EdgeBasedDetectorParameters
	{
	public:
		EdgeBasedDetectorParameters()
		{
			K = 7;
			MinSegmentLength = 10;
		}
		EdgeBasedDetectorParameters(const EdgeBasedDetectorParameters &cpy)
		{
			K = cpy.K;
			MinSegmentLength = cpy.MinSegmentLength;
		}
		EdgeBasedDetectorParameters(int pK, int pMinSegmentLength)
		{
			K = pK;
			MinSegmentLength = pMinSegmentLength;
		}

		int K;
		int MinSegmentLength;

	};

	class EdgeBasedDuneDetector : public BaseDuneDetector
	{
	public:
		EdgeBasedDuneDetector();
		EdgeBasedDuneDetector(BaseImageProcessor* imgproc, const EdgeBasedDetectorParameters &params);
		~EdgeBasedDuneDetector();

		std::vector<DuneSegment> Extract(const cv::Mat &img);

	private:

		EdgeBasedDetectorParameters Parameters;

		double GetDominantOrientation(const cv::Mat &inputImg, const cv::Mat &edges);
		cv::Mat FilterByDominantOrientationUsingKMeans(const cv::Mat &inputImg, const cv::Mat &edges);

		std::vector<std::vector<cv::Point>> GetContours(const cv::Mat &img);
	};

}
#endif