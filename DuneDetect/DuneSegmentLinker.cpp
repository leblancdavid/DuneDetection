#include "DuneSegmentLinker.h"
#include <queue>

namespace dune
{
	DuneSegmentLinker::DuneSegmentLinker()
	{

	}

	DuneSegmentLinker::~DuneSegmentLinker()
	{

	}

	DuneSegmentLinker::DuneSegmentLinker(const SegmentLinkParameters &params)
	{
		Parameters = params;
	}

	DuneSegmentLinker::DuneSegmentLinker(const DuneSegmentLinker &cpy)
	{
		cpy.Parameters;
		distanceTable = cpy.distanceTable.clone();
		linkTable = cpy.linkTable.clone();
	}

	std::vector<DuneSegment> DuneSegmentLinker::GetLinkedSegments(const std::vector<DuneSegment> &inputs)
	{
		return GetLinkedSegments(inputs, Parameters);
	}

	std::vector<DuneSegment> DuneSegmentLinker::GetLinkedSegments(const std::vector<DuneSegment> &inputs, const SegmentLinkParameters &params)
	{
		//This will init the distance table and the link tables, which allows us to find the links between segments
		InitTables(inputs, params);
		
		//Updates the link table so that only true neighbors get linked.
		UpdateLinkTable();

		//This will link all the input DuneSegments which should be linked based on the distance link tables.
		return GetLinkedSegmentFromTables(inputs);
	}

	DuneSegment DuneSegmentLinker::Link(const DuneSegment &seg1, const DuneSegment &seg2, LinkingMethod method, LinkType type)
	{
		if (method == LinkingMethod::EndPointToEndPoint)
		{
			return LinkUsingEndPointToEndPoint(seg1, seg2, type);
		}
		else //For now the only linking method that is supported is endpoint to endpoint
		{
			return LinkUsingEndPointToEndPoint(seg1, seg2, type);
		}
	}

	DuneSegment DuneSegmentLinker::LinkUsingEndPointToEndPoint(const DuneSegment &seg1, const DuneSegment &seg2, LinkType type)
	{
		std::vector<cv::Point> ep1 = seg1.GetEndPoints();
		std::vector<cv::Point> ep2 = seg2.GetEndPoints();

		//If either of them don't have endpoints to link we can't link them.
		if (ep1.empty())
		{
			return seg2;
		}
		else if (ep2.empty())
		{
			return seg1;
		}

		int i1, i2;
		double dist = DuneSegment::GetEndPointDistance(seg1, seg2, i1, i2);

		//Assign the seg1 segment data
		std::vector<DuneSegmentData> joinedSegment = seg1.GetSegmentData();
		std::vector<DuneSegmentData> sd2 = seg2.GetSegmentData();
		
		double xIncr = (double)(ep2[i2].x - ep1[i1].x) / fabs(dist - 1.0); //distance - 1 should never be negative, but just in case, fabs it
		double yIncr = (double)(ep2[i2].y - ep1[i1].y) / fabs(dist - 1.0);

		//Add the link between the two segments
		cv::Point2d pt = cv::Point2d((double)ep1[i1].x + xIncr, (double)ep1[i1].y + yIncr);
		cv::Point linkPt = cv::Point(std::ceil(pt.x - 0.5), std::ceil(pt.y - 0.5));
		double d = 1.0;
		while (linkPt != ep2[i2] && d < dist)
		{
			joinedSegment.push_back(DuneSegmentData(linkPt, 0));
			pt.x += xIncr;
			pt.y += yIncr;

			linkPt = cv::Point(std::ceil(pt.x - 0.5), std::ceil(pt.y - 0.5));
			d++;
		}

		//Add the remaining points from the second segment
		for (size_t i = 0; i < sd2.size(); ++i)
		{
			joinedSegment.push_back(sd2[i]);
		}

		//Return the new segment
		return DuneSegment(joinedSegment);
	}


	void DuneSegmentLinker::InitTables(const std::vector<DuneSegment> &inputs, const SegmentLinkParameters &params)
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
					double d = DuneSegment::GetEndPointDistance(inputs[i], inputs[j]);
					distanceTable.at<double>(i, j) = d;
					distanceTable.at<double>(j, i) = d; //it's symetric matrix
					//The markedMat will mark which segments will need to be linked.
					//A 1 in the markedMat means we need to link the ith and jth segments. 
					if (d < params.DistanceThreshold)
					{
						linkTable.at<uchar>(i, j) = 1;
						linkTable.at<uchar>(j, i) = 1;
					}
				}
			}
		}
	}

	void DuneSegmentLinker::UpdateLinkTable()
	{
		//For each segment
		for (int i = 0; i < linkTable.rows; ++i)
		{
			//Find the nearest segment to it.
			double minDist = DBL_MAX;
			int minIndex = -1, minRow;
			std::vector<int> neighbors;
			//Find all it's neighbors
			for (int j = 0; j < linkTable.cols; ++j)
			{
				if (linkTable.at<uchar>(i, j))
				{
					neighbors.push_back(j);
				}
			}

			//For each of my neighbors
			for (int j = 0; j < (int)neighbors.size(); ++j)
			{
				//Search that neighbors to see which neighbors we have in common
				for (int k = 0; k < (int)neighbors.size(); ++k)
				{
					//if my neighbor 'j' is also a neighbor to 'k' (another one of my neighbors),
					if (linkTable.at<uchar>(neighbors[k], neighbors[j]))
					{
						//and 'j' is closer to 'k' than I am to 'k' and 'j', then they are "better" neighbors
						//so I need to remove 'j' and 'k' from my neighbors
						if (distanceTable.at<double>(neighbors[k], neighbors[j]) < distanceTable.at<double>(neighbors[k], i) &&
							distanceTable.at<double>(i, neighbors[j]) < distanceTable.at<double>(neighbors[k], i))
						{
							//'j' is closer to 'k' than I am, so I will remove both j and k from my neighbors
							linkTable.at<uchar>(neighbors[k], i) = 0;
							linkTable.at<uchar>(i, neighbors[k]) = 0;
							linkTable.at<uchar>(neighbors[j], i) = 0;
							linkTable.at<uchar>(i, neighbors[j]) = 0;
						}
						//Otherwise, I am closer to 'k' than 'j' is, so 'j' and 'k' can no longer be neighbors
						else //those will get fixed when I get there?
						{

						}
					}
				}
			}
		}
	}

	std::vector<DuneSegment> DuneSegmentLinker::GetLinkedSegmentFromTables(const std::vector<DuneSegment> &inputs)
	{
		//Search each segment
		std::vector<DuneSegment> outputs;
		std::vector<bool> isLinked(linkTable.rows);
		for (size_t i = 0; i < isLinked.size(); ++i)
		{
			isLinked[i] = false;
		}

		for (int i = 0; i < linkTable.rows; ++i)
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
				for (int j = 0; j < linkTable.cols; ++j)
				{
					if (linkTable.at<uchar>(row, j))
					{
						//mark the neighbors I've added
						linkTable.at<uchar>(row, j) = 0;
						linkTable.at<uchar>(j, row) = 0;

						bfs.push_back(j);
						isLinked[j] = true;
					}
				}
			}

			DuneSegment linkedSegment = inputs[i];

			for (size_t j = 1; j < toLink.size(); ++j)
			{
				linkedSegment = Link(linkedSegment, inputs[toLink[j]], Parameters.Method, Parameters.Type);
			}

			outputs.push_back(linkedSegment);
			
		}

		return outputs;

	}
}


