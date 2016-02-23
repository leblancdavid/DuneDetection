#include "LaplacianScalePyramid.h"

LaplacianScalePyramid::LaplacianScalePyramid()
{
	init();
}

LaplacianScalePyramid::~LaplacianScalePyramid()
{

}

void LaplacianScalePyramid::Process(const cv::Mat &img)
{
	int k = 21;
	
	cv::Mat scale = img.clone();
	double sigma = 1.0;
	/*_numScales = 0;
	while (scale.rows >= k && scale.cols >= k)
	{
		cv::Mat gaussian, laplace;
		cv::Mat resized;
		
		cv::GaussianBlur(scale, gaussian, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);
		cv::Laplacian(gaussian, laplace, CV_64F, k);
		
		cv::resize(laplace, resized, cv::Size(img.cols, img.rows));

		_pyramid.push_back(resized.clone());
		_sigmaValues.push_back(sigma);

		sigma *= 1.4142;
		cv::resize(scale, scale, cv::Size(scale.cols / 2.0, scale.rows / 2.0));
		_numScales++;
	}*/

	_numScales = 16;
	for (int i = 0; i < _numScales; ++i)
	{
		double scaleFactor = (double)(_numScales - i) / (double)_numScales;
		cv::resize(img, scale, cv::Size(img.cols * scaleFactor, img.rows * scaleFactor));

		cv::Mat gaussian, laplace;
		cv::Mat resized;

		//cv::GaussianBlur(scale, gaussian, cv::Size(k, k), (double)k / 5.0, (double)k / 5.0);
		cv::Laplacian(scale, laplace, CV_64F, k);

		cv::resize(laplace, resized, cv::Size(img.cols, img.rows));

		_pyramid.push_back(resized.clone());
		_sigmaValues.push_back(sigma);

		sigma += 1.4142;

		//cv::imshow("laplace", laplace);
		//cv::waitKey(30);
		
	}

	computeScaleMap();

}

double LaplacianScalePyramid::GetScaleAt(int x, int y)
{
	assert(x < _scaleMap.cols && x >= 0 && y < _scaleMap.rows && y >= 0);

	return _scaleMap.at<double>(y, x);
}



void LaplacianScalePyramid::init()
{

}

void LaplacianScalePyramid::computeScaleMap()
{
	_scaleMap = cv::Mat(_pyramid[0].rows, _pyramid[0].cols, CV_64F);
	_scaleMap = cv::Scalar(1.0);
	std::vector<double> sigmas;
	for (int y = 0; y < _pyramid[0].rows; ++y)
	{
		for (int x = 0; x < _pyramid[0].cols; ++x)
		{
			std::vector<double> lap(_numScales);
			for (int i = 0; i < _numScales; ++i)
			{
				lap[i] = _pyramid[i].at<double>(y, x);
			}

			double max = -99999999999999.9;
			for (int i = 1; i < _numScales - 1; ++i)
			{
				if (lap[i] > max &&
					lap[i] >= lap[i - 1] &&
					lap[i] >= lap[i + 1])
				{
					double sigma = solveScale(cv::Point2d(_sigmaValues[i - 1], lap[i - 1]),
						cv::Point2d(_sigmaValues[i], lap[i]),
						cv::Point2d(_sigmaValues[i + 1], lap[i + 1]));
					_scaleMap.at<double>(y, x) = sigma;
					max = lap[i];
					break;
				}
			}

			//sigmas.push_back(_scaleMap.at<double>(y, x));
		}
	}

	/*double min = 0.0, max = 0.0;
	cv::minMaxIdx(_scaleMap, &min, &max);

	for (int y = 0; y < _scaleMap.rows; ++y)
	{
		for (int x = 0; x < _scaleMap.cols; ++x)
		{
			_scaleMap.at<double>(y, x) = min + max - _scaleMap.at<double>(y, x);
		}
	}*/

	/*cv::Mat norm;
	cv::normalize(_scaleMap, norm, 0.0, 1.0, cv::NORM_MINMAX);
	cv::imshow("norm", norm);
	cv::waitKey(0);*/

}

double LaplacianScalePyramid::solveScale(cv::Point2d pl, cv::Point2d p0, cv::Point2d pr)
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
	double x = (pr.y - pl.y) / (pl.y + p0.y + pr.y);
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