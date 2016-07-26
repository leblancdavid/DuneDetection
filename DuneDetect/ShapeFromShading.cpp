
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

		cv::Mat ShapeFromShadingTsaiShah::Process(const cv::Mat &image, int iterations, int kSize, cv::Mat &P, cv::Mat &Q)
		{
			double albedo, tilt, slant;
			cv::Vec3d illumination;

			estimateAlbedoIllumination(image, albedo, illumination, tilt, slant);
			//surface normals
			P = cv::Mat::zeros(image.size(), CV_32F);
			Q = cv::Mat::zeros(image.size(), CV_32F);
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
						float pVal = P.at<float>(i, j);
						float qVal = Q.at<float>(i, j);
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
				cv::Sobel(Z, P, CV_32F, 1, 0, kSize);
				cv::Sobel(Z, Q, CV_32F, 0, 1, kSize);

				float maxMag = 0.0f;
				for (int i = 0; i < image.rows; ++i)
				{
					for (int j = 0; j < image.cols; ++j)
					{
						float dx, dy;
						dx = P.at<float>(i, j);
						dy = Q.at<float>(i, j);

						float mag = std::sqrt(dx*dx + dy*dy) + 0.01; //to avoid dividing by 0
						dx /= mag;
						dy /= mag;

						if (mag > maxMag)
							maxMag = mag;
					}
				}
				for (int i = 0; i < image.rows; ++i)
				{
					for (int j = 0; j < image.cols; ++j)
					{
						P.at<float>(i, j) /= maxMag;
						Q.at<float>(i, j) /= maxMag;
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

			int k = 7;
			cv::Mat derivX, derivY;
			cv::Sobel(image, derivX, CV_64F, 1, 0, k);
			cv::Sobel(image, derivY, CV_64F, 0, 1, k);

			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					double e = (double)image.at<uchar>(i, j) / 255.0;
					double dx, dy;
					dx = derivX.at<double>(i, j);
					dy = derivY.at<double>(i, j);

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
			slant = std::acos(2.0 * mu1 / gamma); //should be 4.0 but range for acos is -1 to 1, so 4.0 caused undefines.
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

		ShapeFromShadingGradient::ShapeFromShadingGradient()
		{

		}

		ShapeFromShadingGradient::~ShapeFromShadingGradient()
		{

		}

		ShapeFromShadingGradient::ShapeFromShadingGradient(const ShapeFromShadingGradient &cpy)
		{

		}

		cv::Mat ShapeFromShadingGradient::Process(const cv::Mat &image, int K, cv::Mat &P, cv::Mat &Q)
		{
			cv::Sobel(image, P, CV_32F, 1, 0, K);
			cv::Sobel(image, Q, CV_32F, 0, 1, K);

			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					//make all the gradients unit length;
					float dx = P.at<float>(i, j);
					float dy = Q.at<float>(i, j);
					float mag = std::sqrt(dx*dx + dy*dy); 

					if (mag < 0.01) //avoid div by 0
					{
						P.at<float>(i, j) = 0.0f;
						Q.at<float>(i, j) = 0.0f;
					}
					else 
					{
						P.at<float>(i, j) = dx / mag;
						Q.at<float>(i, j) = dy / mag;
					}

				}
			}

			return DepthFromGradients(P, Q);
		}

		cv::Mat DepthFromGradients(const cv::Mat &P, const cv::Mat &Q)
		{
			int N = P.rows;
			int M = P.cols;
			cv::Mat P_period(N * 2, M * 2, CV_32F);
			cv::Mat Q_period(N * 2, M * 2, CV_32F);
			cv::Mat u(N * 2, M * 2, CV_32F);
			cv::Mat v(N * 2, M * 2, CV_32F);

			int ii, jj;
			int ui, vj;
			float signP, signQ;
			for (int i = 0; i < P_period.rows; ++i)
			{
				ii = i;
				ui = i + 1;
				signQ = 1.0f;
				if (ii >= N)
				{
					ii = P_period.rows - i - 1;
					ui = ii + 1.0f;  //-1.0f * ii - 1.0f; //N - i - 1;//
					signQ = -1.0f;
				}
					

				for (int j = 0; j < P_period.cols; ++j)
				{
					jj = j;
					vj = j + 1;
					signP = 1.0f;
					if (jj >= M)
					{
						jj = P_period.cols - j - 1;
						vj = jj + 1.0f; //-1.0f * jj - 1.0f; //M - j - 1;// 
						signP = -1.0f;
					}	

					P_period.at<float>(i, j) = signP * P.at<float>(ii, jj);
					Q_period.at<float>(i, j) = signQ * Q.at<float>(ii, jj);
					u.at<float>(i, j) = (float)ui;
					v.at<float>(i, j) = (float)vj;
				}
			}

			//cv::normalize(u, u, 0.0, 1.0, cv::NORM_MINMAX);
			//cv::imshow("P_period", P_period);
			//cv::waitKey(0);

			cv::Mat p_x[] = { cv::Mat::zeros(P_period.size(), CV_32F), cv::Mat_<float>(P_period) };
			cv::Mat q_y[] = { cv::Mat::zeros(Q_period.size(), CV_32F), cv::Mat_<float>(Q_period) };
			cv::Mat Fp, Fq;
			cv::merge(p_x, 2, Fp);
			cv::merge(q_y, 2, Fq);

			//compute the forward DFT
			cv::dft(Fp, Fp);
			cv::dft(Fq, Fq);

			//split(Fp, p_x);
			//split(Fq, q_y);
			//cv::magnitude(p_x[0], p_x[1], Fp);
			//cv::magnitude(q_y[0], q_y[1], Fq);

			cv::Mat Fz = cv::Mat::zeros(Fp.rows, Fp.cols, CV_32FC2);

			//Apply the math to the complex
			float c = -1.0f / (CV_2PI);
			for (int i = 0; i < Fz.rows; ++i)
			{
				for (int j = 0; j < Fz.cols; ++j)
				{
					float u_n = u.at<float>(i, j) / (float)N;
					float v_m = v.at<float>(i, j) / (float)M;
					float val = c * (u_n * Fp.at<cv::Vec2f>(i, j)[0] +
									 v_m * Fq.at<cv::Vec2f>(i, j)[0]) /
											(u_n*u_n + v_m*v_m);

					/*float val = c * (u_n * Fp.at<float>(i, j) +
						v_m * Fq.at<float>(i, j)) /
						(u_n*u_n + v_m*v_m);*/

					Fz.at<cv::Vec2f>(i, j)[1] = val;
				}
			}

			//do the inverse DFT
			cv::dft(Fz, Fz, cv::DFT_INVERSE);

			//get the real component of the inverse
			cv::split(Fz, q_y);
			//we will want to keep only quadrant 1 (remove the periodic part).
			cv::Mat Z(N, M, CV_32F);
			for (int i = 0; i < N; ++i)
			{
				for (int j = 0; j < M; ++j)
				{
					//Z.at<float>(i, j) = q_y[0].at<float>(q_y[0].rows - i - 1, j);
					Z.at<float>(i, j) = q_y[0].at<float>(i, q_y[0].cols - j - 1);
				}
			}

			//Z = q_y[0];
			cv::normalize(Z, Z, 0.0, 1.0, cv::NORM_MINMAX);

			return Z;
		}

	}
}