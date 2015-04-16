#include "WatershedDuneDetector.h"

namespace dune
{

WatershedDuneDetector::WatershedDuneDetector()
{
	ImageProcess = new WatershedImageProcessor();
}

WatershedDuneDetector::WatershedDuneDetector(WatershedImageProcessor* imgproc, const WatershedDuneDetectorParameters &params)
{
	ImageProcess = imgproc;
	Parameters = params;
}

WatershedDuneDetector::~WatershedDuneDetector()
{

}

std::vector<DuneSegment> WatershedDuneDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	//cv::imshow("Processed", processedImage);
	//cv::waitKey(0);

	std::vector<DuneSegment> duneSegs;
	std::vector<std::vector<cv::Point>> contours = GetContours(processedImage);

	//cv::Mat colorImg;
	//cv::cvtColor(img, colorImg, CV_GRAY2BGR);
	//cv::drawContours(colorImg, contours, -1, cv::Scalar(0, 0, 255), 2);


	for (size_t i = 0; i < contours.size(); ++i)
	{
		std::vector<DuneSegmentData> segData;
		DuneSegment s;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			segData.push_back(DuneSegmentData(contours[i][j], 0));
			//cv::circle(colorImg, contours[i][j], 1, cv::Scalar(255, 0, 0), 2);
		}
		s.SetSegmentData(segData);
		duneSegs.push_back(s);
	}

	//cv::imshow("Contours", colorImg);
	//cv::waitKey(0);

	return duneSegs;
}

std::vector<std::vector<cv::Point>> WatershedDuneDetector::GetContours(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int i = 0;
	std::vector<std::vector<cv::Point>>::iterator ctrIt = contours.begin();
	while (i < (int)contours.size())
	{
		if ((int)contours[i].size() < Parameters.MinSegmentLength)
		{
			//I for some reason don't remember how to use iterators...
			contours.erase(contours.begin() + i);
		}
		else
		{
			i++;
		}
	}

	std::vector<std::vector<cv::Point>> outputSegments = FilterSegmentsByGradients(contours);

	return outputSegments;
}

std::vector<std::vector<cv::Point>> WatershedDuneDetector::FilterSegmentsByGradients(std::vector<std::vector<cv::Point>> &contours)
{
	std::vector<cv::Point> allPoints;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			allPoints.push_back(contours[i][j]);
		}
	}

	WatershedImageProcessor* wsImgProc = dynamic_cast<WatershedImageProcessor*>(ImageProcess);
	double domOrientation = wsImgProc->FindDominantOrientation(DominantOrientationMethod::HOG, Parameters.HistogramBins);
	//double domOrientation = wsImgProc->FindDominantOrientation(DominantOrientationMethod::HOG, allPoints, Parameters.HistogramBins);

	//domOrientation = 0.0;

	std::vector<std::vector<cv::Point>> output;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		if (contours[i].size() > Parameters.MinSegmentLength)
		{
			std::vector<std::vector<cv::Point>> filtered = SplitContourToSegmentsByGradients(domOrientation, contours[i]);
			for (size_t j = 0; j < filtered.size(); ++j)
			{
				if (filtered[j].size() > Parameters.MinSegmentLength)
				{
					output.push_back(std::vector<cv::Point>(filtered[j]));
				}
					
			}
		}
		
	}

	return output;
}

std::vector<std::vector<cv::Point>> WatershedDuneDetector::SplitContourToSegmentsByGradients(double domOrientation, std::vector<cv::Point> &contour)
{
	WatershedImageProcessor* wsImgProc = dynamic_cast<WatershedImageProcessor*>(ImageProcess);
	std::vector<cv::Point> segment;
	std::vector<bool> candidate(contour.size());
	for (size_t j = 0; j < contour.size(); ++j)
	{
		double dir = wsImgProc->BaseData.Gradient.at<cv::Vec4d>(contour[j])[GRADIENT_MAT_DIRECTION_INDEX];

		double diff = std::fabs(dir - domOrientation);
		if (diff < Parameters.AngleTolerance)
		{
			candidate[j] = true;
		}
		else
		{
			candidate[j] = false;
		}
	}

	//This process essentially does a median filtering on the the segment
	std::vector<bool> filtered(candidate.size());
	for (int i = 0; i < contour.size(); ++i)
	{
		int count = 0;
		for (int j = 0, k = i - Parameters.MinSegmentLength/2; j < Parameters.MinSegmentLength; ++j, ++k)
		{
			if (k < 0)
				k = candidate.size() + k;
			if (k >= candidate.size())
				k = k - candidate.size();

			if (candidate[k])
				count++;
		}

		if (count > Parameters.MinSegmentLength / 2)
		{
			filtered[i] = true;
		}
		else
		{
			filtered[i] = false;
		}
	}
	std::vector<std::vector<cv::Point>> output;
	
	int end = filtered.size() - 1;
	if (filtered[0])
	{
		while (end >= 0 && filtered[end])
		{
			segment.push_back(contour[end]);
			end--;
		}
	}
	//Next we want to find contiguous segments.
	for (int i = 0; i < end; ++i)
	{
		while (!filtered[i] && i < end)
		{
			i++;
		}

		while (filtered[i] && i < end)
		{
			segment.push_back(contour[i]);
			i++;
		}

		if (!filtered[i] && segment.size() > Parameters.MinSegmentLength)
		{
			output.push_back(std::vector<cv::Point>(segment));
			segment.clear();
		}
		
	}

	return output;
}

}