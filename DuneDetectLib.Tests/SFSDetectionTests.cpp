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
			dune::SFSImageProcessor sfs;
			
			params.Iterations = 200;
			//params.LightVector = cv::Vec2f(-0.9, 0.3);
			params.LightVector = cv::Vec2f(0, -1.0);
			//params.LightVector /= cv::norm(params.LightVector);

			sfs.SetParameters(&params);

			cv::Mat input = cv::imread(WDC_IMAGE_1, 0);

			cv::medianBlur(input, input, 7);
			cv::GaussianBlur(input, input, cv::Size(15, 15), 3.0, 3.0);
			cv::Mat output;
			sfs.Process(input, output);

			

			cv::Mat t;
			output.convertTo(t, CV_8UC1, 255);
			cv::medianBlur(t, t, 7);
			cv::imwrite("sampleOutput.jpg", t);

			cv::threshold(t, t, 50, 255, CV_THRESH_BINARY_INV);
			//cv::adaptiveThreshold(t, t, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 101, -10.0);

			cv::imshow("input", input);
			cv::imshow("output", output);
			cv::imshow("t", t);
			cv::waitKey(0);
		}
	private:

	};
}