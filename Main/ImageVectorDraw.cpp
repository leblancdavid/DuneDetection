#include "ImageVectorDraw.h"


void ImageVectorDraw::mouseCallback(int event, int x, int y, int flags, void* userdata)
{
	ImageVectorDraw *self = reinterpret_cast<ImageVectorDraw*>(userdata);
	self->doMouseCallback(event, x, y, flags);
}

void ImageVectorDraw::doMouseCallback(int event, int x, int y, int flags)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		if (newVector)
		{
			currentVector.start.x = x;
			currentVector.start.y = y;

			cv::circle(display, currentVector.start, 3, cv::Scalar(0, 255, 0), 2);

			cv::imshow(windowName, display);

			newVector = false;
		}
		else
		{
			currentVector.end.x = x;
			currentVector.end.y = y;
			computerVectorFeatures(currentVector);
			vectors.push_back(VectorPlot(currentVector));

			cv::line(display, currentVector.start, currentVector.end, cv::Scalar(0, 255, 0), 2);

			cv::imshow(windowName, display);

			newVector = true;
		}
		
	}
}

void ImageVectorDraw::Begin(const cv::Mat &inputImage)
{
	newVector = true;

	image = inputImage;
	cv::cvtColor(image, display, CV_GRAY2BGR);

	cv::Sobel(image, derivX, CV_64F, 1, 0, 5);
	cv::Sobel(image, derivY, CV_64F, 0, 1, 5);

	cv::namedWindow(windowName, 1);

	cv::setMouseCallback(windowName, mouseCallback, this);

	cv::imshow(windowName, display);
	cv::waitKey(0);
}

void ImageVectorDraw::computerVectorFeatures(VectorPlot &v)
{
	v.ComputeLength();

	cv::Vec2d direction;
	double mag = std::sqrt((v.end.x - v.start.x)*(v.end.x - v.start.x) + (v.end.y - v.start.y)*(v.end.y - v.start.y));
	direction[0] = (v.end.x - v.start.x) / mag;
	direction[1] = (v.end.y - v.start.y) / mag;

	double d = std::atan2(direction[1], direction[0]);
	double x_incr = std::cos(d);
	double y_incr = std::sin(d);

	v.intensity.clear();
	v.angle.clear();

	for (int i = 0; i < v.length; ++i)
	{
		double x = (double)v.start.x + x_incr * (double)i;
		double y = (double)v.start.y + y_incr * (double)i;

		v.intensity.push_back(image.at<uchar>(y, x));
		
		cv::Vec2d deriv;
		deriv[0] = derivX.at<double>(y, x);
		deriv[1] = derivY.at<double>(y, x);
		double norm = std::sqrt(deriv[0] * deriv[0] + deriv[1] * deriv[1]);

		deriv[0] /= norm;
		deriv[1] /= norm;
		
		v.angle.push_back(deriv[0] * direction[0] + deriv[1] * direction[1]);
		v.magnitude.push_back(norm);
	}

	cv::normalize(v.intensity, v.intensity, 0.0, 1.0, cv::NORM_MINMAX);
	cv::normalize(v.magnitude, v.magnitude, 0.0, 1.0, cv::NORM_MINMAX);
	cv::normalize(v.angle, v.angle, 0.0, 1.0, cv::NORM_MINMAX);
}
