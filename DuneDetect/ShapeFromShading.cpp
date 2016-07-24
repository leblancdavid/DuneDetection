
#include "ShapeFromShading.h"

#define MAX(a,b)        (((a) < (b)) ? (b) : (a))

namespace dune
{
	namespace imgproc
	{
		ShapeFromShadingTsaiShah::ShapeFromShadingTsaiShah()
		{

		}

		ShapeFromShadingTsaiShah::~ShapeFromShadingTsaiShah()
		{

		}

		ShapeFromShadingTsaiShah::ShapeFromShadingTsaiShah(const ShapeFromShadingTsaiShah &cpy)
		{

		}

		cv::Mat ShapeFromShadingTsaiShah::Process(const cv::Mat &image, const cv::Vec2f &lightSource, int iterations)
		{
			float Ps, Qs, p, q, pq, PQs, fZ, dfZ, Eij, Wn = 0.0001*0.0001, Y, K;

			Ps = lightSource[0];
			Qs = lightSource[1];
			PQs = 1.0f + Ps*Ps + Qs*Qs;

			cv::Mat Zn(image.rows, image.cols, CV_32F);
			cv::Mat Si(image.rows, image.cols, CV_32F);

			/* assume the initial estimate zero at time n-1 */
			cv::Mat Zn1 = cv::Mat::zeros(image.rows, image.cols, CV_32F);
			cv::Mat Si1(image.rows, image.cols, CV_32F);
			Si1 = cv::Scalar(0.01);
			
			//Begin
			for (int iter = 0; iter < iterations; ++iter)
			{
				for (int i = 0; i < image.rows; ++i)
				{
					for (int j = 0; j < image.cols; ++j)
					{
						if (j - 1 < 0 || i - 1 < 0)  /* take care boundary */
							p = q = 0.0f;
						else
						{
							p = Zn1.at<float>(i, j) - Zn1.at<float>(i, j - 1);
							q = Zn1.at<float>(i, j) - Zn1.at<float>(i - 1, j);
						}

						pq = 1.0 + p*p + q*q;

						Eij = (float)image.at<uchar>(i, j) / 255.0f;

						fZ = -1.0*(Eij - MAX(0.0, (1 + p*Ps + q*Qs) / (sqrt(pq)*sqrt(PQs))));

						dfZ = -1.0*((Ps + Qs) / (sqrt(pq)*sqrt(PQs)) - (p + q)*(1.0 + p*Ps + q*Qs) /
							(sqrt(pq*pq*pq)*sqrt(PQs)));

						Y = fZ + dfZ* Zn1.at<float>(i,j);
						K = Si1.at<float>(i,j) * dfZ / (Wn + dfZ*Si1.at<float>(i,j) * dfZ);
						Si.at<float>(i,j) = (1.0 - K*dfZ)*Si1.at<float>(i,j);
						Zn.at<float>(i,j) = Zn1.at<float>(i,j) + K*(Y - dfZ*Zn1.at<float>(i,j));
					}
				}
			}

			cv::normalize(Zn, Zn, 0.0, 1.0, cv::NORM_MINMAX);

			return Zn;
		}

	}
}