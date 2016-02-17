#include "GaussianScalePyramid.h"

#define SQRT_2 1.4142

GaussianScalePyramid::GaussianScalePyramid()
{
	//init();
}

GaussianScalePyramid::~GaussianScalePyramid()
{

}

void GaussianScalePyramid::Process(const cv::Mat &img)
{
	/*_pyramid[0] = img;
	for (int i = 0; i < _numScales; ++i)
	{
	cv::GaussianBlur(img, _pyramid[i+1], cv::Size(_kernelSizes[i], _kernelSizes[i]), _sigmaValues[i]);
	}*/

	cv::Mat scale = img.clone();
	double sigma = 1.0;
	_numScales = 0;
	while (scale.rows > 2 && scale.cols > 2)
	{
		cv::Mat resized;
		cv::resize(scale, resized, cv::Size(img.cols, img.rows));

		_pyramid.push_back(resized.clone());
		_sigmaValues.push_back(sigma);

		sigma *= SQRT_2;

		cv::GaussianBlur(scale, scale, cv::Size(3, 3), 1.2, 1.2);
		cv::resize(scale, scale, cv::Size(scale.cols / 2.0, scale.rows / 2.0));
		_numScales++;
	}

	computeScaleMap();

}

double GaussianScalePyramid::GetScaleAt(int x, int y)
{
	assert(x < _scaleMap.cols && x >= 0 && y < _scaleMap.rows && y >= 0);
	
	return _scaleMap.at<double>(y, x);
}

void GaussianScalePyramid::computeScaleMap()
{
	_scaleMap = cv::Mat(_pyramid[0].rows, _pyramid[0].cols, CV_64F);
	_scaleMap = cv::Scalar(1.0);
	_scaleImage = _pyramid[0].clone();

	int k = 5;
	for (int y = 0; y < _pyramid[0].rows; ++y)
	{
		for (int x = 0; x < _pyramid[0].cols; ++x)
		{
			std::vector<double> dog(_numScales);
			dog[0] = 0.0;
			for (int i = 1; i < _numScales; ++i)
			{
				//dog[i] = (double)_pyramid[i].at<uchar>(y, x) -
				//	(double)_pyramid[i-1].at<uchar>(y, x);
				dog[i] = GetLocalPyrVal(i, x, y, k) - GetLocalPyrVal(i - 1, x, y, k);

			}
			double max = -99999999999.9;
			bool scaleFound = false;
			int i;
			for (i = 1; i < _numScales - 1; ++i)
			{
				if (dog[i] > max &&
					dog[i] >= dog[i - 1] &&
					dog[i] >= dog[i + 1])
				{
					double sigma = subPixelCalc(cv::Point2d(_sigmaValues[i - 1], dog[i - 1]),
						cv::Point2d(_sigmaValues[i], dog[i]),
						cv::Point2d(_sigmaValues[i + 1], dog[i + 1]));
					_scaleMap.at<double>(y, x) = sigma;

					double subpix = subPixelCalc(
						cv::Point2d(i - 1, _pyramid[i - 1].at<uchar>(y, x)),
						cv::Point2d(i, _pyramid[i].at<uchar>(y, x)),
						cv::Point2d(i + 1, _pyramid[i + 1].at<uchar>(y, x)));
					int ceil = std::ceil(subpix);
					double f = (double)ceil - subpix;
					uchar val = _pyramid[ceil].at<uchar>(y, x)*(1.0 - f) + _pyramid[ceil - 1].at<uchar>(y, x)*(f);
					_scaleImage.at<uchar>(y, x) = val;

					max = dog[i];
					scaleFound = true;
 				}
			}

			if (!scaleFound)
			{
				//this is a special case... hopefully this works.
				double sigma = subPixelCalc(cv::Point2d(_sigmaValues[i - 1], dog[i - 1]),
					cv::Point2d(_sigmaValues[i], dog[i]),
					cv::Point2d(_sigmaValues[i] * SQRT_2, 0.0));
				_scaleMap.at<double>(y, x) = sigma;
				_scaleImage.at<uchar>(y, x) = _pyramid[i].at<uchar>(y, x);

			}
				
		}
	}

	//_scaleMap += 5.0;
	cv::GaussianBlur(_scaleMap, _scaleMap, cv::Size(5, 5), 1.5, 1.5);
	cv::GaussianBlur(_scaleImage, _scaleImage, cv::Size(5, 5), 1.5, 1.5);
}


double GaussianScalePyramid::subPixelCalc(cv::Point2d pl, cv::Point2d p0, cv::Point2d pr)
{
	//Apparently this method requires the y values to be positive???
	double normVal = 1.0;
	if (pl.y <= 0.0)
	{
		normVal += std::fabs(pl.y);
	}
	if (p0.y <= 0.0)
	{
		normVal += std::fabs(p0.y);
	}
	if (pr.y <= 0.0)
	{
		normVal += std::fabs(pr.y);
	}
	pl.y += normVal;
	p0.y += normVal;
	pr.y += normVal;

	//Sobel Filtered edge???
	//double x = (pr.y - pl.y) / (2.0 * p0.y);

	//center of gravity
	double t = (pl.y + p0.y + pr.y);
	double x = (pr.y - pl.y) / t;
	if (x >= 0)
	{
		x *= std::fabs(pr.x - p0.x);
	}
	else
	{
		x *= std::fabs(p0.x - pl.x);
	}
	return p0.x + x;
}

double GaussianScalePyramid::GetLocalPyrVal(int index, int xi, int yi, int k)
{
	int ki = k / 2;
	int sum = 0, count = 0;
	for (int x = xi - ki; x < xi + ki; ++x)
	{
		if (x < 0 || x >= _pyramid[index].cols)
			continue;
		for (int y = yi - ki; y < yi + ki; ++y)
		{
			if (y < 0 || y >= _pyramid[index].rows)
				continue;

			sum += _pyramid[index].at<uchar>(y, x);
			count++;
		}
	}

	return (double)sum / (double)count;
}

