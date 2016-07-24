
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

		cv::Mat ShapeFromShadingTsaiShah::Process(const cv::Mat &image, int iterations)
		{
			double albedo, tilt, slant;
			cv::Vec3d illumination;

			estimateAlbedoIllumination(image, albedo, illumination, tilt, slant);
			//surface normals
			cv::Mat p = cv::Mat::zeros(image.size(), CV_32F);
			cv::Mat q = cv::Mat::zeros(image.size(), CV_32F);
			//the surface;
			cv::Mat Z = cv::Mat::zeros(image.size(), CV_32F);

			float ix = std::cos(tilt) * std::tan(slant);
			float iy = std::sin(tilt) * std::tan(slant);

			for (int k = 0; k < iterations; ++k)
			{
				for (int i = 0; i < image.rows; ++i)
				{
					for (int j = 0; j < image.cols; ++j)
					{
						float E = (float)image.at<uchar>(i, j) / 255.0f;
						float pVal = p.at<float>(i, j);
						float qVal = q.at<float>(i, j);
						// using the illumination direction and the currently estimate
						// surface normals, compute the corresponding reflectance map.
						float R = (illumination[2] + pVal * illumination[0] + qVal * illumination[1]) /
												std::sqrt(1.0f + pVal*pVal + qVal*qVal);
						//at each iteration, make sure that the reflectance map is positive at
						// each pixel, set negative values to zero.
						if (R < 0.0f)
							R = 0.0f;

						//compute our function f which is the deviation of the computed
						//reflectance map from the original image ...
						float f = E - R;
						//compute the derivative of f with respect to our surface Z
						float df_dZ = (pVal + qVal) *
							(ix*pVal + iy*qVal + 1.0f) /
							(std::sqrt((1.0f + pVal*pVal + qVal*qVal)*(1.0f + pVal*pVal + qVal*qVal)*(1.0f + pVal*pVal + qVal*qVal))*
							std::sqrt(1.0f + ix*ix + iy*iy)) -
							(ix + iy) / (std::sqrt(1.0f + pVal*pVal + qVal*qVal)*
							std::sqrt(1.0f + ix*ix + iy*iy));
						//update our surface
						//avoid dividing by zero
						if (df_dZ < 0.0f)
							df_dZ -= 0.01;
						else
							df_dZ += 0.01;
						Z.at<float>(i, j) -= f / (df_dZ); 
					}
				}

				//compute the surface derivatives with respect to x and y
				//using the updated surface, compute new surface normals
				for (int i = 0; i < image.rows; ++i)
				{
					for (int j = 0; j < image.cols; ++j)
					{
						float zx, zy;
						if (j - 1 < 0)
							zx = 0.0f;
						else
							zx = Z.at<float>(i, j) - Z.at<float>(i, j - 1);

						if (i - 1 < 0)
							zy = 0.0;
						else
							zy = Z.at<float>(i, j) - Z.at<float>(i - 1, j);

						p.at<float>(i, j) = Z.at<float>(i, j) - zx;
						q.at<float>(i, j) = Z.at<float>(i, j) - zy;
					}
				}
			}

			cv::normalize(Z, Z, 0.0, 1.0, cv::NORM_MINMAX);

			return Z;
		}

		void ShapeFromShadingTsaiShah::estimateAlbedoIllumination(const cv::Mat &image, double &albedo, cv::Vec3d &illumination, double &tilt, double &slant)
		{
			double mu1 = 0.0;
			double mu2 = 0.0;
			double muDx = 0.0;
			double muDy = 0.0;
			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					double e = (double)image.at<uchar>(i, j) / 255.0;
					double dx, dy;
					if (j - 1 < 0)
						dx = 0.0;
					else
						dx = e - (double)image.at<uchar>(i, j-1) / 255.0;

					if (i - 1 < 0)
						dy = 0.0;
					else
						dy = e - (double)image.at<uchar>(i-1, j) / 255.0;

					mu1 += e;
					mu2 += e*e;

					double mag = std::sqrt(dx*dx + dy*dy) + 0.01; //to avoid dividing by 0
					muDx += dx / mag;
					muDy += dy / mag;
				}
			}

			double nPixels = (double)(image.rows*image.cols);
			mu1 /= nPixels;
			mu2 /= nPixels;
			muDx /= nPixels;
			muDy /= nPixels;

			double gamma = std::sqrt((6.0 * CV_PI*CV_PI * mu2) - (48.0 * mu1*mu1));
			albedo = gamma / CV_PI;
			slant = std::acos(3.5 * mu1 / gamma); //should be 4.0 but range for acos is -1 to 1, so 4.0 caused undefines.
			tilt = atan2(muDy, muDx);
			if (tilt < 0)
				tilt += CV_PI;

			illumination[0] = std::cos(tilt)*std::sin(slant);
			illumination[1] = std::sin(tilt)*std::sin(slant);
			illumination[2] = std::cos(slant);
		}


		//cv::Mat ShapeFromShadingTsaiShah::Process(const cv::Mat &image, const cv::Vec2f &lightSource, int iterations)
		//{
		//	float Ps, Qs, p, q, pq, PQs, fZ, dfZ, Eij, Wn = 0.0001*0.0001, Y, K;

		//	Ps = lightSource[0];
		//	Qs = lightSource[1];
		//	PQs = 1.0f + Ps*Ps + Qs*Qs;

		//	cv::Mat Zn(image.rows, image.cols, CV_32F);
		//	cv::Mat Si(image.rows, image.cols, CV_32F);

		//	/* assume the initial estimate zero at time n-1 */
		//	cv::Mat Zn1 = cv::Mat::zeros(image.rows, image.cols, CV_32F);
		//	Zn1 = cv::Scalar(0.01);
		//	cv::Mat Si1(image.rows, image.cols, CV_32F);
		//	Si1 = cv::Scalar(0.01);
		//	
		//	//Begin
		//	for (int iter = 0; iter < iterations; ++iter)
		//	{
		//		for (int i = 0; i < image.rows; ++i)
		//		{
		//			for (int j = 0; j < image.cols; ++j)
		//			{
		//				if (j - 1 < 0 || i - 1 < 0)  /* take care boundary */
		//					p = q = 0.0f;
		//				else
		//				{
		//					p = Zn1.at<float>(i, j) - Zn1.at<float>(i, j - 1);
		//					q = Zn1.at<float>(i, j) - Zn1.at<float>(i - 1, j);
		//				}

		//				pq = 1.0 + p*p + q*q;

		//				Eij = (float)image.at<uchar>(i, j) / 255.0f;

		//				fZ = -1.0*(Eij - MAX(0.0, (1.0 + p*Ps + q*Qs) / (sqrt(pq)*sqrt(PQs))));

		//				dfZ = -1.0*((Ps + Qs) / (sqrt(pq)*sqrt(PQs)) - (p + q)*(1.0 + p*Ps + q*Qs) /
		//					(sqrt(pq*pq*pq)*sqrt(PQs)));

		//				Y = fZ + dfZ* Zn1.at<float>(i,j);
		//				K = Si1.at<float>(i,j) * dfZ / (Wn + dfZ*Si1.at<float>(i,j) * dfZ);
		//				Si.at<float>(i,j) = (1.0 - K*dfZ)*Si1.at<float>(i,j);
		//				Zn.at<float>(i,j) = Zn1.at<float>(i,j) + K*(Y - dfZ*Zn1.at<float>(i,j));
		//			}
		//		}

		//		for (int i = 0; i < image.rows; ++i)
		//		{
		//			for (int j = 0; j < image.cols; ++j)
		//			{
		//				float check = Si.at<float>(i, j);

		//				Zn1.at<float>(i,j) = Zn.at<float>(i,j);
		//				Si1.at<float>(i,j) = Si.at<float>(i,j);

		//				
		//			}
		//		}
		//	}

		//	cv::normalize(Si, Si, 0.0, 1.0, cv::NORM_MINMAX);

		//	return Si;
		//}

	}
}