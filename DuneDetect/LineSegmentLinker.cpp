#include "LineSegmentLinker.h"

namespace dune
{
	LineSegmentLinker::LineSegmentLinker()
	{

	}

	LineSegmentLinker::~LineSegmentLinker()
	{

	}


	LineSegmentLinker::LineSegmentLinker(const LineSegmentLinker &cpy)
	{
		cpy.Parameters;
		distanceTable = cpy.distanceTable.clone();
		linkTable = cpy.linkTable.clone();
	}

	std::vector<DuneSegment> LineSegmentLinker::GetLinkedSegments(const std::vector<DuneSegment> &inputs)
	{
		InitTables(inputs);

		return GetLinkedSegmentFromTables(inputs);
	}

	DuneSegment LineSegmentLinker::Link(const DuneSegment &seg1, const DuneSegment &seg2)
	{
		std::vector<DuneSegmentData> s1 = seg1.GetSegmentData();
		std::vector<DuneSegmentData> s2 = seg2.GetSegmentData();
		for (size_t i = 0; i < s2.size(); ++i)
		{
			s1.push_back(s2[i]);
		}

		DuneSegment output;
		output.SetSegmentData(s1);

		return output;
	}

	void LineSegmentLinker::InitTables(const std::vector<DuneSegment> &inputs)
	{
		distanceTable = cv::Mat(inputs.size(), inputs.size(), CV_64F);
		distanceTable = cv::Scalar(-1.0);
		linkTable = cv::Mat(inputs.size(), inputs.size(), CV_8U);
		linkTable = cv::Scalar(0);
		//First compute the distance matrix. This matrix holds the distance between all segments
		for (size_t i = 0; i < inputs.size(); ++i)
		{
			for (size_t j = 0; j < inputs.size(); ++j)
			{
				if (i == j)
				{
					distanceTable.at<double>(i, j) = DBL_MAX; //don't want to link to itself
				}
				else if (distanceTable.at<double>(i, j) < 0.0)
				{
					double d = DuneSegment::GetLineDistance(inputs[i], inputs[j]);
					distanceTable.at<double>(i, j) = d;
					distanceTable.at<double>(j, i) = d; //it's symetric matrix
					//The markedMat will mark which segments will need to be linked.
					//A 1 in the markedMat means we need to link the ith and jth segments. 
					/*if (d < params.DistanceThreshold)
					{
						linkTable.at<uchar>(i, j) = 1;
						linkTable.at<uchar>(j, i) = 1;
					}*/
				}
			}
		}
	}

	std::vector<DuneSegment> LineSegmentLinker::GetLinkedSegmentFromTables(const std::vector<DuneSegment> &inputs)
	{
		//Search each segment
		std::vector<DuneSegment> outputs;
		std::vector<bool> isLinked(distanceTable.rows);
		for (size_t i = 0; i < isLinked.size(); ++i)
		{
			isLinked[i] = false;
		}

		for (int i = 0; i < distanceTable.rows; ++i)
		{
			//if it's already been linked, we don't want to repeat this process and have duplicates
			if (isLinked[i])
				continue;
			std::vector<int> toLink;
			std::vector<int> bfs;
			bfs.push_back(i);
			//Begin bfs
			while (bfs.size() > 0)
			{
				int row = bfs.front();
				toLink.push_back(row);
				bfs.erase(bfs.begin());

				//Push all the neighbors
				for (int j = 0; j < distanceTable.cols; ++j)
				{
					if (distanceTable.at<double>(row, j) > 0.98)
					{
						//mark the neighbors I've added
						distanceTable.at<double>(row, j) = 0;
						distanceTable.at<double>(j, row) = 0;

						bfs.push_back(j);
						isLinked[j] = true;
					}
				}
			}

			DuneSegment linkedSegment = inputs[i];

			for (size_t j = 1; j < toLink.size(); ++j)
			{
				linkedSegment = Link(linkedSegment, inputs[toLink[j]]);
			}

			outputs.push_back(linkedSegment);

		}

		return outputs;
	}
}


