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
			
			params.Iterations = 40;
			params.LightVector = cv::Vec3f(0.9559, -0.2935, 0.5);

			sfs.SetParameters(&params);

			cv::Mat input = cv::imread(KALAHARI_IMAGE_1, 0);
			cv::Mat output;
			sfs.Process(input, output);

			cv::imshow("output", output);
			cv::waitKey(0);
		}
	private:

	};
}