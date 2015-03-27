#include "EdgeBasedDuneDetector.h"

#include <fstream>

namespace dune
{
EdgeBasedDuneDetector::EdgeBasedDuneDetector()
{
	ImageProcess = new EdgeDetectorImageProcessor();
}

EdgeBasedDuneDetector::EdgeBasedDuneDetector(BaseImageProcessor* imgproc, const EdgeBasedDetectorParameters &params)
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

	//double dom = GetDominantOrientation(img, processedImage);
	//cv::Mat orientationImg;
	//cv::cvtColor(img, orientationImg, CV_GRAY2BGR);
	//cv::Point center(orientationImg.cols / 2.0, orientationImg.rows / 2.0);
	//cv::Point endPt(100.0*std::cos(dom) + center.x, 100.0*std::sin(dom) + center.y);
	//cv::line(orientationImg, center, endPt, cv::Scalar(0, 0, 255), 2);
	//cv::imshow("Average Orientation", orientationImg);
	//cv::waitKey(0);

	cv::Mat filtered = FilterByDominantOrientationUsingHoG(img, processedImage);
	std::vector<std::vector<cv::Point>> contours = GetContours(processedImage);

	cv::Mat colorImg;
	cv::cvtColor(img, colorImg, CV_GRAY2BGR);
	

	std::vector<DuneSegment> duneSegs;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		double validPts = 0;
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			if (filtered.at<uchar>(contours[i][j]) != 0)
			{
				validPts++;
			}
		}

		double r = validPts / (double)contours[i].size();
		if (r > Parameters.R)
		{
			DuneSegment s;
			for (size_t j = 0; j < contours[i].size(); ++j)
			{
				s.Data.push_back(DuneSegmentData(contours[i][j], 0));
				colorImg.at<cv::Vec3b>(contours[i][j]) = cv::Vec3b(255, 0, 0);
			}
			duneSegs.push_back(s);
		}
	}

	cv::imshow("Contours", colorImg);
	cv::waitKey(30);

	return duneSegs;
}

double EdgeBasedDuneDetector::GetDominantOrientation(const cv::Mat &inputImg, const cv::Mat &edges)
{

	return 0.0;
}

cv::Mat EdgeBasedDuneDetector::FilterByDominantOrientationUsingHoG(const cv::Mat &inputImg, const cv::Mat &edges)
{
	cv::Mat sobel_x;
	cv::Mat sobel_y;

	cv::Sobel(inputImg, sobel_x, CV_64F, 1, 0, Parameters.K);
	cv::Sobel(inputImg, sobel_y, CV_64F, 0, 1, Parameters.K);

	double mean_dx = 0, mean_dy = 0, mean_mag = 0, avgOrientation, u_x = 0.0, u_y = 0.0;
	double sum = 0;
	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			if (edges.at<uchar>(y, x))
			{
				double dx = sobel_x.at<double>(y, x);
				double dy = sobel_y.at<double>(y, x);
				mean_dx += dx / 1000.0;
				mean_dy += dy / 1000.0;
				mean_mag += std::sqrt(dx*dx + dy*dy) / 1000.0;
				sum++;
			}
		}
	}

	mean_dx /= sum;
	mean_dx *= 1000.0;
	mean_dy /= sum;
	mean_dy *= 1000.0;
	mean_mag /= sum;
	mean_mag *= 1000.0;
	avgOrientation = std::atan2(mean_dy, mean_dx);

	if (avgOrientation < 0)
		avgOrientation += 2.0*3.1416;

	//for (int x = 0; x < inputImg.cols; ++x)
	//{
	//	for (int y = 0; y < inputImg.rows; ++y)
	//	{
	//		//if (edges.at<uchar>(y, x))
	//		//{
	//			u_x += (sobel_x.at<double>(y, x) - mean_dx) / 1000.0;
	//			u_y += (sobel_y.at<double>(y, x) - mean_dy) / 1000.0;
	//		//}
	//	}
	//}

	//avgOrientation = std::atan2(u_y, u_x);

	if (avgOrientation < 0)
		avgOrientation += 2.0*3.1416;

	double increments = 2.0*3.1416 / (double)Parameters.HistogramBins;
	std::vector<double> HoG(Parameters.HistogramBins);
	std::vector<cv::Point2d> HoG2D(Parameters.HistogramBins);
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		HoG2D[i].x = 0.0;
		HoG2D[i].y = 0.0;
	}
		

	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			if (edges.at<uchar>(y, x))
			{
				double dx = sobel_x.at<double>(y, x) - mean_dx;
				double dy = sobel_y.at<double>(y, x) - mean_dy;
				double mag = std::sqrt(dx*dx + dy*dy);

				double orientation = std::atan2(dy, dx) - avgOrientation;
				while (orientation < 0)
					orientation += 2.0*3.1416;

				int bin = std::ceil((orientation / increments) - 0.5);
				if (bin >= Parameters.HistogramBins)
					bin = 0;

				HoG2D[bin].x += dx / 1000.0;
				HoG2D[bin].y += dy / 1000.0;

				//HoG[bin] += std::sqrt(dx*dx + dy*dy);
			}
		}
	}

	double maxMag = 0;
	double maxOrientation = 0;
	int index = 0;
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		double dx = HoG2D[i].x / 1000.0;
		double dy = HoG2D[i].y / 1000.0;
		HoG[i] = std::sqrt(dx*dx + dy*dy);
		if (HoG[i] > maxMag)
		{
			maxMag = HoG[i];
			maxOrientation = std::atan2(dy, dx);
			while (maxOrientation < 0)
				maxOrientation += 2.0*3.1416;
			index = i;
		}
	}

	cv::Mat filteredImg = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
	cv::Mat resultsImg;
	cv::cvtColor(inputImg, resultsImg, CV_GRAY2BGR);

	for (int x = 0; x < inputImg.cols; ++x)
	{
		for (int y = 0; y < inputImg.rows; ++y)
		{
			if (edges.at<uchar>(y, x))
			{
				double dx = sobel_x.at<double>(y, x) - mean_dx;
				double dy = sobel_y.at<double>(y, x) - mean_dx;

				double orientation = std::atan2(dy, dx);
				while (orientation < 0)
					orientation += 2.0*3.1416;

				if (std::fabs(orientation - maxOrientation) < Parameters.AngleTolerance ||
					std::fabs(orientation - maxOrientation + 2.0*3.1416) < Parameters.AngleTolerance)
				{
					filteredImg.at<uchar>(y, x) = 255;
					resultsImg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 255, 0);
				}
				else
				{
					resultsImg.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
				}
				/*int bin = std::ceil((orientation / increments) - 0.5);
				if (bin >= Parameters.HistogramBins)
					bin = 0;

				if (bin == index)
				{
					filteredImg.at<uchar>(y, x) = 255;
				}*/
			}
		}
	}

	//cv::imshow("resultImg", resultsImg);
	//cv::waitKey(0);

	/*cv::Point center(resultsImg.cols / 2.0, resultsImg.rows / 2.0);
	cv::Point endPt(150.0*std::cos(maxOrientation) + center.x, 150.0*std::sin(maxOrientation) + center.y);
	cv::Point arrow1(30.0*std::cos(maxOrientation - 2.3562) + endPt.x, 30.0*std::sin(maxOrientation - 2.3562) + endPt.y);
	cv::Point arrow2(30.0*std::cos(maxOrientation - 3.927) + endPt.x, 30.0*std::sin(maxOrientation - 3.927) + endPt.y);
	cv::line(resultsImg, center, endPt, cv::Scalar(0, 0, 255), 7);
	cv::line(resultsImg, endPt, arrow1, cv::Scalar(0, 0, 255), 7);
	cv::line(resultsImg, endPt, arrow2, cv::Scalar(0, 0, 255), 7);

	cv::imwrite("hog_unnorm_results.jpg", resultsImg);
	std::ofstream hogfile("hog_unnorm_results.txt");
	for (size_t i = 0; i < HoG.size(); ++i)
	{
		hogfile << HoG[i] << std::endl;
	}
	hogfile.close();
*/
	//cv::Mat orientationImg;
	//cv::cvtColor(inputImg, orientationImg, CV_GRAY2BGR);
	//cv::Point center(orientationImg.cols / 2.0, orientationImg.rows / 2.0);
	//cv::Point endPt(200.0*std::cos(domOrientation) + center.x, 200.0*std::sin(domOrientation) + center.y);
	//cv::Point arrow1(20.0*std::cos(domOrientation - 2.3562) + endPt.x, 20.0*std::sin(domOrientation - 2.3562) + endPt.y);
	//cv::Point arrow2(20.0*std::cos(domOrientation - 3.927) + endPt.x, 20.0*std::sin(domOrientation - 3.927) + endPt.y);
	//cv::line(orientationImg, center, endPt, cv::Scalar(0, 0, 255), 3);
	//cv::line(orientationImg, endPt, arrow1, cv::Scalar(0, 0, 255), 3);
	//cv::line(orientationImg, endPt, arrow2, cv::Scalar(0, 0, 255), 3);
	//cv::imwrite("orientationImg.jpg", orientationImg);
	////cv::imshow("Average Orientation", orientationImg);
	////cv::waitKey(1000);

	return filteredImg;
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
	cv::imwrite("orientationImg.jpg", orientationImg);
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

std::vector<std::vector<cv::Point>> EdgeBasedDuneDetector::GetContours(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

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

	return contours;
}

}