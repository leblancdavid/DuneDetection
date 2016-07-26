/********************************************************/
/* This is the source code for the simple SFS algorithm */
/* It will read the UCF format image, and output the    */
/* estimated depth maps as a raw data file.             */
/* It can be complied as follow:			*/
/*  	cc -g -o shading shading.c -lm			*/
/********************************************************/
//
//This program implements Tsai - Shah method for shape from shading.
//The algorithm is described in the following tech report :
//
//
//The following tech report is available for anonymous ftp.
//Please post this in vision list, thanks.
//
//A FAST LINEAR SHAPE FROM SHADING
//
//PING - SING TSAI AND MUBARAK SHAH
//DEPARTMENT OF COMPUTER SCIENCE
//UNIVERSITY OF CENTRAL FLORIDA
//ORLANDO, FL 32816
//
//Abstract
//In this paper, we present an extremely simple algorithm for   shape from
//shading, which can be implemented in 25 lines of C code(code included),
//and which converges in one to two iterations.
//The algorithm is very fast, taking .2 seconds on a Sun SparcStation - 1 for a
//$128 \times 128$ image, and is purely local and highly parallelizable
//(parallel implementation included).The algorithm gives a solution which
//is proveably convergent and unique, and is general in that it works for
//both the Lambertian and Specular reflectance functions.In our approach, we
//employ a linear approximation of the reflectance function, as used by others.
//However, the major difference is that we first use the discrete approximations for
//surface normal, $p$ and $q$, using finite differences, and
//then linearize the reflectance function
//in depth, $Z(x, y)$, instead of $p$ and $q$.The algorithm has been
//tested on several synthetic and real images of both Lambertian and
//specular surfaces, and good results have been obtained.
//It gives good results even for the spherical surfaces, in contrast to
//other linear methods.
//
//______________________
//ftp eustis.cs.ucf.edu
//Connected to eustis.
//220 eustis FTP server(SunOS 4.1) ready.
//Name(eustis.cs.ucf.edu:shah) : anonymous
//331 Guest login ok, send ident as password.
//Password :
//		 230 Guest login ok, access restrictions apply.
//		 ftp> cd pub
//		 250 CWD command successful.
//		 ftp> get tec24.ps.Z
//		 200 PORT command successful.
//		 150 ASCII data connection for tec24.ps.Z(132.170.108.100, 1184) (1138595 bytes).
//		 226 ASCII Transfer complete.
//	 local: tec24.ps.Z remote : tec24.ps.Z
//			1142749 bytes received in 11 seconds(99 Kbytes / s)
//			ftp> quit
//			221 Goodbye.
//			uncompress tec24.ps
//			lpr tec24.ps - Plw
//			______________________
//
//
//			The images are in ucfimgs subdirectory.
//			Please send email to tsai@eola.cs.ucf.edu for any problems and
//			comments.
//
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include "opencv2\opencv.hpp"

using namespace std;

namespace dune
{
	namespace imgproc
	{
		class ShapeFromShadingTsaiShah
		{
		public:
			ShapeFromShadingTsaiShah();
			~ShapeFromShadingTsaiShah();
			ShapeFromShadingTsaiShah(const ShapeFromShadingTsaiShah &cpy);

			cv::Mat Process(const cv::Mat &image, int iterations, int k);

		private:
			void estimateAlbedoIllumination(const cv::Mat &image, double &albedo, cv::Vec3d &illumination, double &tilt, double &slant);
		};

		class ShapeFromShadingGradient
		{
		public:
			ShapeFromShadingGradient();
			~ShapeFromShadingGradient();
			ShapeFromShadingGradient(const ShapeFromShadingGradient &cpy);

			cv::Mat Process(const cv::Mat &image, int K, cv::Mat &P, cv::Mat &Q);

		private:
			cv::Mat depthFromGradients(const cv::Mat &P, const cv::Mat &Q);
		};
	}
}
