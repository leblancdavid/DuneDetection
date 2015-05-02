#ifndef _CONNECTED_COMPONENTS_EXTRACTOR_H_
#define _CONNECTED_COMPONENTS_EXTRACTOR_H_

#include "OpenCVHeaders.h"

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

				return components;
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
