#include "EdgeBasedDuneDetector.h"
#include "DuneSegmentLinker.h"
#include "ConnectedComponentsExtractor.h"

#include <fstream>

namespace dune
{
EdgeBasedDuneDetector::EdgeBasedDuneDetector()
{
	ImageProcess = new EdgeDetectorImageProcessor();
}

EdgeBasedDuneDetector::EdgeBasedDuneDetector(EdgeDetectorImageProcessor* imgproc, const EdgeBasedDetectorParameters &params)
{
	ImageProcess = imgproc;
	Parameters = params;
}

EdgeBasedDuneDetector::~EdgeBasedDuneDetector()
{

}

std::vector<DuneSegment> EdgeBasedDuneDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	cv::Mat crestlineImg = FilterByDominantOrientation(processedImage);

	std::vector<DuneSegment> duneSegs;
	duneSegs = GetDuneSegmentContours(crestlineImg);

	if (Parameters.ApplyLinking)
	{
		std::vector<DuneSegment> linkedSegs = LinkDuneSegments(duneSegs);
		duneSegs = FilterSegmentsByMagnitude(duneSegs);
		return linkedSegs;
	}
	return duneSegs;
}

double EdgeBasedDuneDetector::GetDominantOrientation(const cv::Mat &inputImg, const cv::Mat &edges)
{

	return 0.0;
}

cv::Mat EdgeBasedDuneDetector::FilterByDominantOrientation(const cv::Mat &edges)
{
	EdgeDetectorImageProcessor* edgeImgProc = dynamic_cast<EdgeDetectorImageProcessor*>(ImageProcess);
	double domOrientation = edgeImgProc->FindDominantOrientation(DominantOrientationMethod::HOG, Parameters.HistogramBins);
	cv::Mat filtered = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
	for (int x = 0; x < edges.cols; ++x)
	{
		for (int y = 0; y < edges.rows; ++y)
		{
			if (edges.at<uchar>(y, x))
			{
				double dir = edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(y,x)[GRADIENT_MAT_DIRECTION_INDEX];

				double diff = std::fabs(dir - domOrientation);
				if (diff < Parameters.AngleTolerance)
				{
					filtered.at<uchar>(y, x) = 255;
				}
			}
		}
	}

	return filtered;
}

cv::Mat EdgeBasedDuneDetector::FilterByDominantOrientationUsingKMeans(const cv::Mat &inputImg, const cv::Mat &edges)
{
	cv::Mat sobel_x;
	cv::Mat sobel_y;

	cv::Sobel(inputImg, sobel_x, CV_32F, 1, 0, Parameters.K);
	cv::Sobel(inputImg, sobel_y, CV_32F, 0, 1, Parameters.K);

	std::vector<cv::Point2f> gradients;
	std::vector<cv::Point> points;
	float avgMag = 0;
	float scale = sobel_x.rows*sobel_y.cols;
	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			if (edges.at<uchar>(y, x))
			{
				float mag = std::sqrt(sobel_x.at<float>(y, x)*sobel_x.at<float>(y, x) +
					sobel_y.at<float>(y, x)*sobel_y.at<float>(y, x)) / 1000.0f;
				avgMag += mag;

				gradients.push_back(cv::Point2f(sobel_x.at<float>(y, x), sobel_y.at<float>(y, x)));
				points.push_back(cv::Point(x, y));
			}
		}
	}

	avgMag /= (float)gradients.size();
	avgMag *= 1000.0f;

	for (size_t i = 0; i < gradients.size(); ++i)
	{
		gradients[i].x /= avgMag;
		gradients[i].y /= avgMag;
	}

	int clusters = 2;
	std::vector<int> labels;
	cv::Mat centers;
	cv::kmeans(gradients, clusters, labels, cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 0.1), 5, cv::KMEANS_PP_CENTERS, centers);

	double domOrientation;
	double maxMag = 0;
	int index = 0;
	for (int i = 0; i < centers.rows; ++i)
	{
		double m = centers.at<float>(i, 0)*centers.at<float>(i, 0) + centers.at<float>(i, 1)*centers.at<float>(i, 1);
		if (m > maxMag)
		{
			maxMag = m;
			domOrientation = std::atan2(centers.at<float>(i, 1), centers.at<float>(i, 0));
			index = i;
		}
	}

	cv::Mat orientationImg;
	cv::cvtColor(inputImg, orientationImg, CV_GRAY2BGR);
	cv::Point center(orientationImg.cols / 2.0, orientationImg.rows / 2.0);
	cv::Point endPt(200.0*std::cos(domOrientation) + center.x, 200.0*std::sin(domOrientation) + center.y);
	cv::Point arrow1(20.0*std::cos(domOrientation - 2.3562) + endPt.x, 20.0*std::sin(domOrientation - 2.3562) + endPt.y);
	cv::Point arrow2(20.0*std::cos(domOrientation - 3.927) + endPt.x, 20.0*std::sin(domOrientation - 3.927) + endPt.y);
	cv::line(orientationImg, center, endPt, cv::Scalar(0, 0, 255), 3);
	cv::line(orientationImg, endPt, arrow1, cv::Scalar(0, 0, 255), 3);
	cv::line(orientationImg, endPt, arrow2, cv::Scalar(0, 0, 255), 3);
	//cv::imwrite("orientationImg.jpg", orientationImg);
	//cv::imshow("Average Orientation", orientationImg);
	//cv::waitKey(1000);



	std::ofstream clusterFile1("clusterFile1.txt");
	std::ofstream clusterFile2("clusterFile2.txt");

	cv::Mat filteredResults = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC3);
	cv::Mat filteredImg = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);

	cv::cvtColor(inputImg, filteredResults, CV_GRAY2BGR);

	for (size_t i = 0; i < labels.size(); ++i)
	{
		if (labels[i] == index)
		{
			filteredResults.at<cv::Vec3b>(points[i]) = cv::Vec3b(0, 255, 0);
			//filteredImg.at<cv::Vec3b>(points[i]) = cv::Vec3b(0, 255, 0);
			filteredImg.at<uchar>(points[i]) = 255;
			clusterFile1 << gradients[i].x << '\t' << gradients[i].y << std::endl;
		}
		else
		{
			//filteredResults.at<cv::Vec3b>(points[i])[2] = 255;
			clusterFile2 << gradients[i].x << '\t' << gradients[i].y << std::endl;
		}
	}

	clusterFile1.close();
	clusterFile2.close();

	//cv::imwrite("filteredResults.jpg", filteredResults);
	//cv::imshow("Processed Img", filteredResults);
	//cv::waitKey(1000);


	return filteredImg;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::GetDuneSegmentContours(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	//cv::Mat img32;
	//img.convertTo(img32, CV_32S);
	//cv::findContours(img32, contours, CV_RETR_FLOODFILL, CV_CHAIN_APPROX_NONE);
	//Find contour DOES NOT do what I expect it to, need to use connected components
	//cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	imgproc::ConnectedComponentsExtractor cce;
	contours = cce.GetCC(img);

	std::vector<DuneSegment> duneSegs;

	for (size_t i = 0; i < contours.size(); ++i)
	{
		std::vector<DuneSegmentData> segData;
		DuneSegment s;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			segData.push_back(DuneSegmentData(contours[i][j], 0));
		}
		s.SetSegmentData(segData);
		duneSegs.push_back(s);
	}

	return duneSegs;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::FilterSegmentsByMagnitude(const std::vector<DuneSegment> &input)
{
	EdgeDetectorImageProcessor* edgeImgProc = dynamic_cast<EdgeDetectorImageProcessor*>(ImageProcess);
	//double meanM = edgeImgProc->BaseData.Mean[GRADIENT_MAT_MAGNITUDE_INDEX];
	//double stdDevM = edgeImgProc->BaseData.StdDev[GRADIENT_MAT_MAGNITUDE_INDEX];

	double meanM = 0.0;
	double count = 0.0;
	double stdDevM = 0.0;
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<cv::Point> pts = input[i].GetEndPoints();
		for (size_t j = 0; j < pts.size(); ++j)
		{
			meanM += edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j])[GRADIENT_MAT_MAGNITUDE_INDEX] / 1000.0;
			count++;
		}
	}
	meanM /= count / 1000.0;

	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<cv::Point> pts = input[i].GetEndPoints();
		for (size_t j = 0; j < pts.size(); ++j)
		{
			stdDevM += fabs(edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j])[GRADIENT_MAT_MAGNITUDE_INDEX] - meanM) / 1000.0;
		}
	}
	stdDevM /= count / 1000.0;

	double threshold = meanM + Parameters.R * stdDevM;

	std::vector<DuneSegment> output;
	for (size_t i = 0; i < input.size(); ++i)
	{
		std::vector<cv::Point> pts = input[i].GetEndPoints();
		double u = 0.0;
		for (size_t j = 0; j < pts.size(); ++j)
		{
			u += edgeImgProc->BaseData.Gradient.at<cv::Vec4d>(pts[j])[GRADIENT_MAT_MAGNITUDE_INDEX] / 1000.0;
		}
		u /= (double)pts.size() / 1000.0;
		if (u > threshold)
		{
			output.push_back(input[i]);
		}
	}

	return output;
}

std::vector<DuneSegment> EdgeBasedDuneDetector::LinkDuneSegments(const std::vector<DuneSegment> &unlinked)
{
	SegmentLinkParameters params;
	params.DistanceThreshold = Parameters.LinkDistance;
	params.Method = EndPointToEndPoint;
	params.Type = Linear;
	DuneSegmentLinker linker(params);

	std::vector<DuneSegment> linked = linker.GetLinkedSegments(unlinked);

	return linked;
}

}