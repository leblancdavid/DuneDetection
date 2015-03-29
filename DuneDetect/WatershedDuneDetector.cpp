#include "WatershedDuneDetector.h"

namespace dune
{

WatershedDuneDetector::WatershedDuneDetector()
{
	ImageProcess = new WatershedImageProcessor();
}

WatershedDuneDetector::WatershedDuneDetector(BaseImageProcessor* imgproc, const WatershedDuneDetectorParameters &params)
{
	ImageProcess = imgproc;
	Parameters = params;
}

WatershedDuneDetector::~WatershedDuneDetector()
{

}

std::vector<DuneSegment> WatershedDuneDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	std::vector<DuneSegment> duneSegs;

	return duneSegs;
}


}