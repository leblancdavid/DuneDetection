#ifndef _SKELETONIZATION_REGION_THINNER_H_
#define _SKELETONIZATION_REGION_THINNER_H_

#include "BaseDuneRegionThinner.h"
#include "ConnectedComponentsExtractor.h"
namespace duneML
{

	class SkeletonizationRegionThinner
	{
	public:

		SkeletonizationRegionThinner()
		{
			C = -4.0;
			upperThreshold = 0.5;
			smoothing = 15;
			minSegmentLength = 25;
		}
		~SkeletonizationRegionThinner() {}

		//Expects a CV_32F Mat
		std::vector<dune::DuneSegment> GetDuneSegments(const cv::Mat &input, const std::string &saveFileName = "")
		{
			cv::Mat processed;
			if (smoothing > 0 && smoothing % 1)
			{
				cv::GaussianBlur(input, processed, cv::Size(smoothing, smoothing), smoothing / 3.0, smoothing / 3.0);
			}
			else
			{
				processed = input;
			}

			cv::Mat img8u(processed.rows, processed.cols, CV_8UC1);
			for (int i = 0; i < img8u.rows; i++)
			{
				for (int j = 0; j < img8u.cols; j++)
				{
					img8u.at<uchar>(i, j) = (processed.at<float>(i, j) + 1.0f) * 127;
				}
			}

			cv::Mat binaryImage;
			//cv::adaptiveThreshold(img8u, binaryImage, 255.0, cv::ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, smoothing*9, C);
			cv::threshold(img8u, binaryImage, 127, 1, CV_THRESH_BINARY);

			if (saveFileName != "")
			{
				cv::Mat scaled = binaryImage * 255;
				cv::imwrite(saveFileName + "_threshold_results.jpg", scaled);
			}

			//cv::imshow("result", img8u);
			//cv::imshow("threshold results", binaryImage);
			//cv::waitKey(0);

			cv::Mat prev = cv::Mat::zeros(input.size(), CV_8UC1);
			cv::Mat diff;

			do {
				thinningIteration(binaryImage, 0);
				thinningIteration(binaryImage, 1);
				cv::absdiff(binaryImage, prev, diff);
				binaryImage.copyTo(prev);
			} while (cv::countNonZero(diff) > 0);

			binaryImage *= 255;
			//cv::imshow("adaptive threshold results", binaryImage);
			//cv::waitKey(0);

			if (saveFileName != "")
			{
				cv::imwrite(saveFileName + "_thin_results.jpg", binaryImage);
			}
			
			//for now save it as one big segment
			//TODO: come up with a way to split up the segments
			std::vector<std::vector<cv::Point>> contours;
			dune::imgproc::ConnectedComponentsExtractor cce;
			contours = cce.GetCC(binaryImage);
			
			std::vector<dune::DuneSegment> duneSegs;

			for (size_t i = 0; i < contours.size(); ++i)
			{
				//ignore all segments which are smaller than the desired minimum length
				if (contours[i].size() < minSegmentLength)
					continue;

				std::vector<dune::DuneSegmentData> segData;
				dune::DuneSegment s;
				for (size_t j = 0; j < contours[i].size(); ++j)
				{
					segData.push_back(dune::DuneSegmentData(contours[i][j], 0));
				}
				s.SetSegmentData(segData);
				duneSegs.push_back(s);
			}


			return duneSegs;
		}

	private:

		double C;
		double upperThreshold;
		int smoothing;
		int minSegmentLength;
		//code from:
		//http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
		//PLEASE REFERENCE PROPERLY
		void thinningIteration(cv::Mat& im, int iter)
		{
			cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

			for (int i = 1; i < im.rows - 1; i++)
			{
				for (int j = 1; j < im.cols - 1; j++)
				{
					uchar p2 = im.at<uchar>(i - 1, j);
					uchar p3 = im.at<uchar>(i - 1, j + 1);
					uchar p4 = im.at<uchar>(i, j + 1);
					uchar p5 = im.at<uchar>(i + 1, j + 1);
					uchar p6 = im.at<uchar>(i + 1, j);
					uchar p7 = im.at<uchar>(i + 1, j - 1);
					uchar p8 = im.at<uchar>(i, j - 1);
					uchar p9 = im.at<uchar>(i - 1, j - 1);

					int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
						(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
						(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
						(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
					int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
					int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
					int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

					if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
						marker.at<uchar>(i, j) = 1;
				}
			}

			im &= ~marker;
		}

	};



}

#endif