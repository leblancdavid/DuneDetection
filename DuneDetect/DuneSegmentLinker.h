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

	class DuneSegmentLinker
	{
	public:
		DuneSegmentLinker();
		~DuneSegmentLinker();
		DuneSegmentLinker(const SegmentLinkParameters &params);
		DuneSegmentLinker(const DuneSegmentLinker &cpy);

		std::vector<DuneSegment> GetLinkedSegments(const std::vector<DuneSegment> &inputs);
		std::vector<DuneSegment> GetLinkedSegments(const std::vector<DuneSegment> &inputs, const SegmentLinkParameters &params);

		static DuneSegment Link(const DuneSegment &seg1, const DuneSegment &seg2, LinkingMethod method, LinkType type);


	private:

		SegmentLinkParameters Parameters;
		cv::Mat distanceTable;
		cv::Mat linkTable;

		static DuneSegment LinkUsingEndPointToEndPoint(const DuneSegment &seg1, const DuneSegment &seg2, LinkType type);
		void InitTables(const std::vector<DuneSegment> &inputs, const SegmentLinkParameters &params);
		void UpdateLinkTable();
		std::vector<DuneSegment> GetLinkedSegmentFromTables(const std::vector<DuneSegment> &inputs);
	};

}




#endif