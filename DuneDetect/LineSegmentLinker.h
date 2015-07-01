#pragma once

#include "BaseSegmentLinker.h"

namespace dune
{
	class LineSegmentLinker : public BaseSegmentLinker
	{
	public:
		LineSegmentLinker();
		~LineSegmentLinker();
		LineSegmentLinker(const LineSegmentLinker &cpy);

		std::vector<DuneSegment> GetLinkedSegments(const std::vector<DuneSegment> &inputs);

		static DuneSegment Link(const DuneSegment &seg1, const DuneSegment &seg2);

	private:

		SegmentLinkParameters Parameters;
		cv::Mat distanceTable;
		cv::Mat linkTable;

		void InitTables(const std::vector<DuneSegment> &inputs);
		void UpdateLinkTable();
		std::vector<DuneSegment> GetLinkedSegmentFromTables(const std::vector<DuneSegment> &inputs);
	};
}
