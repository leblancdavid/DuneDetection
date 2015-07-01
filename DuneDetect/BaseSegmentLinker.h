#ifndef _DUNE_SEGMENT_LINKER_H_
#define _DUNE_SEGMENT_LINKER_H_

#include "OpenCVHeaders.h"
#include "DuneSegment.h"

namespace dune
{
	enum LinkingMethod { EndPointToEndPoint, EndPointToNearest };
	enum LinkType { Linear, Poly, Cubic };

	class SegmentLinkParameters
	{
	public:
		SegmentLinkParameters()
		{
			Method = EndPointToEndPoint;
			Type = Linear;
			DistanceThreshold = 10.0;
		}
		~SegmentLinkParameters() {}
		SegmentLinkParameters(const SegmentLinkParameters &cpy)
		{
			Method = cpy.Method;
			Type = cpy.Type;
			DistanceThreshold = cpy.DistanceThreshold;
		}


		LinkingMethod Method;
		LinkType Type;
		double DistanceThreshold;
	};

	class BaseSegmentLinker
	{
	public:
		BaseSegmentLinker() {}
		~BaseSegmentLinker() {}

		virtual std::vector<DuneSegment> GetLinkedSegments(const std::vector<DuneSegment> &inputs)=0;

		static DuneSegment Link(const DuneSegment &seg1, const DuneSegment &seg2);


	private:

		
	};

}




#endif