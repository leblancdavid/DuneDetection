#pragma once
#include "OpenCVHeaders.h"

class VectorPlot
{
public:
	VectorPlot(){}
	~VectorPlot(){}
	VectorPlot(const VectorPlot &cpy)
	{
		start = cpy.start;
		end = cpy.end;
		for (int i = 0; i < cpy.intensity.size(); ++i)
		{
			intensity.push_back(cpy.intensity[i]);
		}
		for (int i = 0; i < cpy.magnitude.size(); ++i)
		{
			magnitude.push_back(cpy.magnitude[i]);
		}
		for (int i = 0; i < cpy.angle.size(); ++i)
		{
			angle.push_back(cpy.angle[i]);
		}

		length = cpy.length;
	}

	void ComputeLength()
	{
		cv::Vec2d direction;
		direction[0] = (end.x - start.x);
		direction[1] = (end.y - start.y);

		length = std::sqrt(direction[0] * direction[0] + direction[1] * direction[1]);
	}

	cv::Point start;
	cv::Point end;
	double length;
	std::vector<double> intensity;
	std::vector<double> magnitude;
	std::vector<double> angle;
};



class ImageVectorDraw
{
public:
	ImageVectorDraw() {}
	~ImageVectorDraw() {}
	ImageVectorDraw(const ImageVectorDraw &cpy) {}

	void Begin(const cv::Mat &image);

	std::vector<VectorPlot> GetVectors() { return vectors; }
	cv::Mat GetDisplay() { return display; }

private:
	
	const std::string windowName = "Image Vector Draw";
	std::vector<VectorPlot> vectors;

	bool newVector;
	VectorPlot currentVector;
	cv::Mat image, display, derivX, derivY;

	static void mouseCallback(int event, int x, int y, int flags, void* userdata);
	void doMouseCallback(int event, int x, int y, int flags);

	void computerVectorFeatures(VectorPlot &v);


};