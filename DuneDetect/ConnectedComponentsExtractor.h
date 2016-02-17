#ifndef _CONNECTED_COMPONENTS_EXTRACTOR_H_
#define _CONNECTED_COMPONENTS_EXTRACTOR_H_

#include "OpenCVHeaders.h"
#include "ConnectedComponentsSorter.h"

namespace dune
{
	namespace imgproc
	{
		class ConnectedComponentsExtractor
		{
		public:
			ConnectedComponentsExtractor() { }
			~ConnectedComponentsExtractor() { }
			ConnectedComponentsExtractor(const ConnectedComponentsExtractor &cpy)
			{
				Buffer = cpy.Buffer.clone();
			}
			
			std::vector<std::vector<cv::Point>> GetCC(const cv::Mat &image)
			{
				std::vector <std::vector<cv::Point>> components;
				Buffer = image.clone();

				for (int x = 0; x < Buffer.cols; ++x)
				{
					std::vector<cv::Point> c;
					for (int y = 0; y < Buffer.rows; ++y)
					{
						if (Buffer.at<uchar>(y, x))
						{
							//push back the segment
							components.push_back(DFS(cv::Point(x, y)));
						}
					}
				}

				ConnectedComponentsSorter sorter;
				std::vector <std::vector<cv::Point>> output;

				//cv::Mat test = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);

				for (size_t i = 0; i < components.size(); ++i)
				{
					std::vector <std::vector<cv::Point>> sortedPoints = sorter.Sort(components[i]);
					for (size_t j = 0; j < sortedPoints.size(); ++j)
					{
						//random color;
						//cv::Vec3b color;
						//color[0] = rand() % 200 + 50;
						//color[1] = rand() % 200 + 50;
						//color[2] = rand() % 200 + 50;

						output.push_back(std::vector<cv::Point>());
						for (size_t k = 0; k < sortedPoints[j].size(); ++k)
						{
							output[output.size() - 1].push_back(sortedPoints[j][k]);

							//test.at<cv::Vec3b>(sortedPoints[j][k]) = color;
						}
					}
				}

				//cv::dilate(test, test, cv::Mat(), cv::Point(-1, -1), 1);
				//cv::imshow("Segment test", test);
				//cv::imwrite("SegmentationTest.png", test);
				//cv::waitKey(0);

				return output;
			}


		private:

			//Depth first search to get the connected components
			std::vector<cv::Point> DFS(cv::Point seed)
			{
				std::vector<cv::Point> stack;
				stack.push_back(seed);
				std::vector<cv::Point> segment;
				
				//cv::imshow("Buffer", Buffer);
				//cv::waitKey(0);

				while (!stack.empty())
				{
					cv::Point anchor = stack.back();
					stack.pop_back();
					segment.push_back(anchor);
					Buffer.at<uchar>(anchor) = 0;

					for (int x = anchor.x - 1; x <= anchor.x + 1; ++x)
					{
						for (int y = anchor.y - 1; y <= anchor.y + 1; ++y)
						{
							if (x >= 0 &&
								x < Buffer.cols &&
								y >= 0 &&
								y < Buffer.rows)
							{
								if (Buffer.at<uchar>(y, x) > 0)
								{
									Buffer.at<uchar>(y, x) = 0;
									stack.push_back(cv::Point(x, y));
								}
									
							}
						}
					}
				}

				return segment;
			}

			cv::Mat Buffer;
		};
	}

}


#endif
