#ifndef _BASE_DUNE_SEGMENT_H_
#define _BASE_DUNE_SEGMENT_H_

#include "OpenCVHeaders.h"

namespace dune
{
	class DuneSegmentData
	{
	public:
		DuneSegmentData();
		~DuneSegmentData();
		DuneSegmentData(const DuneSegmentData &cpy);
		DuneSegmentData(cv::Point pos, double orient);
		double DistanceFrom(cv::Point pos) const;

		cv::Point Position;
		double Orientation;
	};

	class DuneSegment
	{
	public:
		DuneSegment();
		~DuneSegment();
		DuneSegment(const std::vector<DuneSegmentData> &segment);
		DuneSegment(const DuneSegment &cpy);

		cv::Point FindClosestPoint(const cv::Point &from) const;
		cv::Point FindClosestPoint(const cv::Point &from, double &dist) const;
		cv::Point FindClosestPoint(const cv::Point &from, double &dist, int &index) const;

		static double GetEndPointDistance(const DuneSegment &src, const DuneSegment &dst);
		static double GetEndPointDistance(const DuneSegment &src, const DuneSegment &dst, int &srcIndex, int &dstIndex);
		static double GetLineDistance(const DuneSegment &src, const DuneSegment &dst);

		DuneSegmentData operator [] (int i) const { return Segment[i]; }
		DuneSegmentData& operator [] (int i) { return Segment[i]; }

		std::vector<cv::Point> GetEndPoints() const
		{
			return EndPoints;
		}

		std::vector<DuneSegmentData> GetSegmentData() const
		{
			return Segment;
		}

		int GetLength() const
		{
			return (int)Segment.size();
		}

		cv::Vec4d GetLine() const
		{
			return Line;
		}

		void GaussianSmooth(int kSize, bool wrap=false)
		{
			std::vector<DuneSegmentData> smoothed(Segment.size());
			if (smoothed.size() < kSize)
				return;

			cv::Mat gausK = cv::getGaussianKernel(kSize, kSize / 5.0);

			for (int i = 0; i < Segment.size(); ++i)
			{
				int count = 0;
				double x = 0.0;
				double y = 0.0;
				for (int j = 0; j < kSize; ++j)
				{
					int k = i - kSize / 2 + j;
					if (wrap)
					{
						if (k < 0)
							k = Segment.size() + k;
						else if (k >= (int)Segment.size())
							k = j - 1;
					}
					else
					{
						if (k < 0)
							k = 0;
						else if (k >= (int)Segment.size())
							k = Segment.size()-1;
					}

					x += Segment[k].Position.x * gausK.at<double>(j, 0);
					y += Segment[k].Position.y * gausK.at<double>(j, 0);
				}

				smoothed[i].Position.x = x;
				smoothed[i].Position.y = y;
			}

			Segment = smoothed;
			Fill();
		}

		void Fill(bool wrap = false)
		{
			for (int i = 1; i < Segment.size(); ++i)
			{
				int distance = (int)std::sqrt((Segment[i].Position.x - Segment[i - 1].Position.x)*(Segment[i].Position.x - Segment[i - 1].Position.x) +
					(Segment[i].Position.y - Segment[i - 1].Position.y)*(Segment[i].Position.y - Segment[i - 1].Position.y));
				if (distance > 1)
				{
					double direction = std::atan2(Segment[i].Position.y - Segment[i - 1].Position.y, Segment[i].Position.x - Segment[i - 1].Position.x);
					double dx = std::cos(direction);
					double dy = std::sin(direction);
					std::vector<DuneSegmentData>::iterator it = Segment.begin() + i - 1;
					for (int d = 1.0; d < distance; ++d)
					{
						it++; 
						it = Segment.insert(it,
							DuneSegmentData(cv::Point(Segment[i - 1].Position.x + std::ceil((double)d*dx - 0.5), 
													Segment[i - 1].Position.y + std::ceil((double)d*dy - 0.5)), 0.0));
					}
					i += (int)distance;

				}
				else if (distance == 0.0)
				{
					std::vector<DuneSegmentData>::iterator it = Segment.begin() + i;
					Segment.erase(it);
					i--;
				}
			}
		}

		void SetSegmentData(const std::vector<DuneSegmentData> &segData)
		{
			Segment = segData;
			ComputeEndPoints();
		}

		void ComputeFeatures();
		
	private:

		std::vector<DuneSegmentData> Segment;		
		std::vector<cv::Point> EndPoints;
		cv::Vec4f Line;

		void ComputeEndPoints();
		void FitLine();
		void Validate();
		
	};

}




#endif