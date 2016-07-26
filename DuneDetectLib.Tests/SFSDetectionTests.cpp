#include "stdafx.h"
#include "CppUnitTest.h"

#include "DuneDatasetHelpers.h"

#include "ShapeFromShadingDuneDetector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DuneDetectLibTests
{
	TEST_CLASS(SFSDetectionTests)
	{
	public:

		TEST_METHOD(SFS_DepthMap_Computation_Test)
		{
			std::string testPath = TEST_CASE_DIRECTORY;

			float test = rand();
			dune::SFSProcessParameters params;
			params.Iterations = 200;
			params.K = 5;
			dune::ShapeFromShadingDetectorParameters *parameters = new dune::ShapeFromShadingDetectorParameters();
			dune::SFSImageProcessor sfs;
			sfs.SetParameters(&params);
			dune::BaseDuneDetector *duneDetector = new dune::ShapeFromShadingDetector(&sfs, parameters);
			
			cv::Mat input = cv::imread(KALAHARI_IMAGE_1, 0);

			cv::medianBlur(input, input, 7);
			cv::GaussianBlur(input, input, cv::Size(params.K, params.K), (double)params.K / 6.0, (double)params.K / 6.0);
			cv::Mat output;
			//duneDetector->Extract(input);
			sfs.Process(input, output);

			cv::Mat t;
			output.convertTo(t, CV_8UC1, 255);
			cv::medianBlur(t, t, 5);
			cv::imwrite("sampleOutput.png", t);

			//cv::threshold(t, t, 50, 255, CV_THRESH_BINARY_INV);
			//cv::adaptiveThreshold(t, t, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 101, -10.0);

			cv::imshow("input", input);
			cv::imshow("output", output);
			//cv::imshow("t", t);
			cv::waitKey(0);
		}
	private:

	};
}