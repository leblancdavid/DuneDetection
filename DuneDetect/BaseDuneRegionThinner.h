#ifndef _BASE_DUNE_REGION_THINNER_H_
#define _BASE_DUNE_REGION_THINNER_H_

#include "OpenCVHeaders.h"
#include "DuneSegment.h"

namespace duneML
{

	class BaseDuneRegionThinner
	{
	public:

		BaseDuneRegionThinner()
		{
		}
		virtual ~BaseDuneRegionThinner() {}

		//Expects a CV_32F Mat
		virtual std::vector<dune::DuneSegment> GetDuneSegments(const cv::Mat &input, const std::string &saveFileName = "") = 0;

	private:

	};



}

#endif