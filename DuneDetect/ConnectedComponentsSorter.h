#ifndef _CONNECTED_COMPONENTS_SORTER_H_
#define _CONNECTED_COMPONENTS_SORTER_H_

#include "OpenCVHeaders.h"
#include <list>

namespace dune
{
	namespace imgproc
	{
		class ConnectedSegment
		{
		public:
			ConnectedSegment() 
			{
				first = cv::Point(INT_MIN, INT_MIN);
				last = cv::Point(INT_MIN, INT_MIN);
			}
			~ConnectedSegment() {}

			int InsertLocation(const cv::Point &point)
			{
				if (connectedPoints.empty())
					return 1;
				else
				{
					if (std::abs(point.x - first.x) <= 1 &&
						std::abs(point.y - first.y) <= 1)
						return 1;
					if(std::abs(point.x - last.x) <= 1 &&
						std::abs(point.y - last.y) <= 1)
						return -1;

					if (connectedPoints.size() > 2)
					{
						cv::Point second = *(connectedPoints.begin()++);
						if (std::abs(point.x - second.x) <= 1 &&
							std::abs(point.y - second.y) <= 1)
							return 1;

						std::list<cv::Point>::iterator it = connectedPoints.end();
						it--;
						cv::Point secondToLast = *it;
						if (std::abs(point.x - secondToLast.x) <= 1 &&
							std::abs(point.y - secondToLast.y) <= 1)
							return -1;
					}
					return 0;
				}
			}

			bool Insert(const cv::Point &point)
			{
				int insertWhere = InsertLocation(point);
				if (insertWhere == 0)
					return false;

				//if we haven't inserted last point yet, add it
				if (connectedPoints.size() == 1)
				{
					connectedPoints.insert(connectedPoints.end(), point);
					last = point;
				}
				else if (insertWhere > 0)
				{
					connectedPoints.insert(connectedPoints.begin(), point);
					first = point;
				}
				else
				{
					connectedPoints.insert(connectedPoints.end(), point);
					last = point;
				}
			}

			bool Insert(const std::vector<cv::Point> &points)
			{
				std::vector<bool> pointsAdded(points.size());
				std::vector<int> distances(points.size());
				for (size_t i = 0; i < points.size(); ++i)
				{
					pointsAdded[i] = false;
					distances[i] = Distance(points[i]);
					//if the distance is greater than 2, this won't work
					if (distances[i] > 2)
						return false;
				}

				int counter = 0;
				while (counter != points.size())
				{
					//find the min distance
					int minDist = INT_MAX;
					int minIndex = 0;
					for (size_t i = 0; i < distances.size(); ++i)
					{
						if (!pointsAdded[i] && distances[i] < minDist)
						{
							minDist = distances[i];
							minIndex = i;
						}
					}

					pointsAdded[minIndex] = true;
					Insert(points[minIndex]);
					counter++;
				}

				return true;
			}

			std::vector<cv::Point> ToVector()
			{
				std::list<cv::Point>::iterator it;
				std::vector<cv::Point> output;
				for (it = connectedPoints.begin(); it != connectedPoints.end(); ++it)
					output.push_back(*it);

				return output;
			}

		private:
			std::list<cv::Point> connectedPoints;
			cv::Point first, last;

			int Distance(const cv::Point &pt)
			{
				int location = InsertLocation(pt);
				if (location == 0)
					return INT_MAX;
				else if (location > 0)
				{
					return std::abs(first.x - pt.x) + std::abs(first.y - pt.y);
				}
				else
				{
					return std::abs(last.x - pt.x) + std::abs(last.y - pt.y);
				}
			}
		};

		class ConnectedComponentsSorter
		{
		public:
			ConnectedComponentsSorter() { }
			~ConnectedComponentsSorter() { }
			ConnectedComponentsSorter(const ConnectedComponentsSorter &cpy)
			{
			}

			std::vector<std::vector<cv::Point>> Sort(const std::vector<cv::Point> &points)
			{
				std::vector<std::vector<cv::Point>> output;
				if (points.size() == 0)
					return output;

				std::vector<ConnectedSegment> segments(1);

				std::vector<bool> addedPoints(points.size());
				for (size_t i = 0; i < addedPoints.size(); ++i)
				{
					addedPoints[i] = false;
				}

				segments[0].Insert(points[0]);
				addedPoints[0] = true;
				int currentIndex = 0;

				bool pointsRemaining = true;
				while (pointsRemaining)
				{
					pointsRemaining = false;
					std::vector<cv::Point> pointsToInsert;
					for (size_t i = 0; i < points.size(); ++i)
					{
						if (addedPoints[i])
							continue;

						pointsRemaining = true;
						if (segments[currentIndex].InsertLocation(points[i]) != 0)
						{
							pointsToInsert.push_back(points[i]);
							addedPoints[i] = true;
						}
					}

					if (!pointsRemaining)
						break;

					if (pointsToInsert.size() == 0)
					{
						segments.push_back(ConnectedSegment());
						currentIndex++;
						int t = 0;
						while (addedPoints[t])
							t++;

						segments[currentIndex].Insert(points[t]); 
						addedPoints[t] = true;
					}
					else
					{
						segments[currentIndex].Insert(pointsToInsert);
						/*for (size_t i = 0; i < pointsToInsert.size(); ++i)
						{
							if (segments[currentIndex].Insert(pointsToInsert[i]) == 0)
							{
								std::cout << "Insert FAIL";
							}
						}*/
					}
				}
				
				for (size_t i = 0; i < segments.size(); ++i)
				{
					std::vector<cv::Point> seg = segments[i].ToVector();
					output.push_back(seg);
				}

				return output;
			}

		private:

		};
	}
}


#endif
