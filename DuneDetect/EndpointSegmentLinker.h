#pragma once

#include "BaseSegmentLinker.h"

namespace dune
{
	class EndpointSegmentLinker : public BaseSegmentLinker
{
public:
	EndpointSegmentLinker();
	~EndpointSegmentLinker();
	EndpointSegmentLinker(const SegmentLinkParameters &params);
	EndpointSegmentLinker(const EndpointSegmentLinker &cpy);

	std::vector<DuneSegment> GetLinkedSegments(const std::vector<DuneSegment> &inputs);

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
