#include "DuneSegmentDetector.h"

namespace dune
{

DuneSegmentDetector::DuneSegmentDetector() 
{
	ImageProcess = new AdaptiveImageProcessor();
	InitCurvatureKernel(Parameters.KernelSize, Parameters.KernelSigma);
	InitGaussianKernel(Parameters.KernelSize, Parameters.KernelSigma);
}

DuneSegmentDetector::DuneSegmentDetector(BaseImageProcessor* imgproc, const DuneSegmentDetectorParameters &params) 
{
	ImageProcess = imgproc;
	Parameters = params;
	InitCurvatureKernel(Parameters.KernelSize, Parameters.KernelSigma);
	InitGaussianKernel(Parameters.KernelSize, Parameters.KernelSigma);
}

DuneSegmentDetector::~DuneSegmentDetector() 
{
	/*if(ImageProcess != NULL)
	{
		delete ImageProcess;
	}*/
}

void DuneSegmentDetector::InitGaussianKernel(int size, double sigma)
{
	GaussianKernel.resize(size);
	double c = 1.0 / (sigma * std::sqrt(2.0*3.14159));
	double sum = 0;
	for(int i = 0, j = -1*size/2; i < size; ++i, ++j)
	{
		GaussianKernel[i] = c * std::exp(-0.5*(double)((j)*(j))/(sigma*sigma));
		sum += GaussianKernel[i];
	}
	//Normalize to a sum of 1.
	for(int i = 0; i < size; ++i)
	{
		GaussianKernel[i] /= sum;
	}

}

void DuneSegmentDetector::InitCurvatureKernel(int size, double sigma)
{
	CurvatureKernel.resize(size);
	double c = 1.0 / (sigma * std::sqrt(2.0*3.14159));
	double sum = 0;
	for(int i = 0, j = -1*size/2; i < size; ++i, ++j)
	{
		CurvatureKernel[i] = c*(1.0 - (double)((j)*(j))/(sigma*sigma))*std::exp(-0.5*(double)((j)*(j))/(sigma*sigma));//assuming a mean of 0
	}
}

std::vector<DuneSegment> DuneSegmentDetector::Extract(const cv::Mat &img)
{
	cv::Mat processedImage;
	ImageProcess->Process(img, processedImage);

	std::vector<DuneSegment> duneSegs;
	std::vector<std::vector<cv::Point>> contours = GetContours(processedImage);

	for(size_t i = 0; i < contours.size(); ++i)
	{
		DuneSegment s;
		for(size_t j = 0; j < contours[i].size(); ++j)
		{
			s.Data.push_back(DuneSegmentData(contours[i][j], 0));
		}
		duneSegs.push_back(s);
	}

	/*cv::Mat colorImg;
	cv::cvtColor(img, colorImg, CV_GRAY2BGR);
	cv::drawContours(colorImg, contours, -1, cv::Scalar(0,0,255),2);
	cv::imshow("Contours", colorImg);
	cv::waitKey(0);*/

	return duneSegs;
}

std::vector<std::vector<cv::Point>> DuneSegmentDetector::GetContours(const cv::Mat &img)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	int i = 0;
	std::vector<std::vector<cv::Point>>::iterator ctrIt = contours.begin();
	while(i < (int)contours.size())
	{
		if((int)contours[i].size() < Parameters.MinSegmentLength)
		{
			//I for some reason don't remember how to use iterators...
			contours.erase(contours.begin()+i);
		}
		else
		{
			i++;
		}
	}

	cv::Mat smoothedImg;
	cv::Mat derivativeImg;
	cv::medianBlur(img, smoothedImg, 9);
	cv::Sobel(img, derivativeImg, CV_64F, 1, 1, 11);
	//cv::Laplacian(img, derivativeImg, CV_64F, 9);
	std::vector<std::vector<cv::Point>> outputSegments;
	for(size_t i = 0; i < contours.size(); ++i)
	{
		std::vector<std::vector<cv::Point>> segments = SplitContourSegments(contours[i]);

		std::vector<double> derivs;
		double averageMag = 0;
		for(size_t j = 0; j < segments.size(); ++j)
		{
			double d = CalcSegmentAverageDerivative(derivativeImg, segments[j]);
			derivs.push_back(d);
			averageMag += d;
		}
		averageMag /= (double)segments.size();

		for(size_t j = 0; j < segments.size(); ++j)
		{
			if(derivs[j] < averageMag)
			{
				GaussianSmoothSegment(segments[j]);
				outputSegments.push_back(segments[j]);
			}
			//GaussianSmoothSegment(segments[j]);
			//outputSegments.push_back(segments[j]);
		}
	}

	return outputSegments;
}

std::vector<std::vector<cv::Point>> DuneSegmentDetector::SplitContourSegments(const std::vector<cv::Point> &contour)
{
	std::vector<std::vector<cv::Point>> segments;
	std::vector<double> curvature = CalcContourCurvature(contour);
	
	std::vector<int> peaks;
	for(int i = 0; i < curvature.size(); ++i)
	{
		int b = i-1;
		int f = i+1;
		if(b < 0)
			b = curvature.size()-1;
		if(f == curvature.size())
			f = 0;

		if(curvature[i] > curvature[b] && curvature[i] > curvature[f])
		{
			peaks.push_back(i);
		}
	}

	std::vector<cv::Point> s;
	for(int i = peaks[peaks.size()-1]; i < contour.size(); ++i)
	{
		s.push_back(contour[i]);
	}
	for(int i = 0; i < peaks[0]; ++i)
	{
		s.push_back(contour[i]);
	}
	segments.push_back(s);

	for(int i = 0; i < peaks.size()-1; ++i)
	{
		int start = peaks[i];
		int end =  peaks[i+1]; 
		s.clear();
		while(start < end)
		{
			s.push_back(contour[start]);
			start++;
		}
		segments.push_back(s);
	}

	return segments;


}

std::vector<double> DuneSegmentDetector::CalcContourCurvature(const std::vector<cv::Point> &contour)
{
	std::vector<cv::Point> cntrTemp = contour;
	std::vector<double> curvature(contour.size());
	for(int i = 0; i < Parameters.KernelSize; ++i)
	{
		cntrTemp.push_back(cntrTemp[i]);
	}
	for(size_t i = 0; i < contour.size(); ++i)
	{
		double c_x = 0;
		double c_y = 0;
		for(size_t j = 0; j < CurvatureKernel.size(); ++j)
		{
			c_x = cntrTemp[i+j].x * CurvatureKernel[j];
			c_y = cntrTemp[i+j].y * CurvatureKernel[j];
		}

		curvature[i] = c_x*c_x + c_y*c_y;
	}

	for(int i = 0; i < Parameters.KernelSize/2; ++i)
	{
		double last = curvature[curvature.size()-1];
		curvature.pop_back();
		curvature.insert(curvature.begin(), last);
	}

	return curvature;

}

void DuneSegmentDetector::GaussianSmoothSegment(std::vector<cv::Point> &segment)
{
	std::vector<cv::Point> smoothed;
	for(int i = 0; i < segment.size(); ++i)
	{
		cv::Point2d avg(0,0);
		for(int j = 0; j < Parameters.KernelSize; ++j)
		{
			int index = i + j - Parameters.KernelSize/2;
			if(index < 0)
				index = 0;
			if(index >= segment.size())
				index = segment.size()-1;

			avg.x += GaussianKernel[j]*segment[index].x;
			avg.y += GaussianKernel[j]*segment[index].y;
		}

		smoothed.push_back(cv::Point((int)std::ceil(avg.x - 0.5), (int)std::ceil(avg.y - 0.5)));
	}

	segment = smoothed;
}

double DuneSegmentDetector::CalcSegmentAverageDerivative(const cv::Mat &derivativeImg, const std::vector<cv::Point> &segment)
{
	double deriv = 0;

	for(size_t i = 0; i < segment.size(); ++i)
	{
		deriv += fabs(derivativeImg.at<double>(segment[i]));
	}

	return deriv / (double)segment.size();

}

}