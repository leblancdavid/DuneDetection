#include "DuneDetector.h"
#include <msclr\marshal_cppstd.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

array<DuneSegment^>^ DuneDetector::GetDuneSegments(System::String^ imageFile)
{
	std::string imageFileCvt = msclr::interop::marshal_as<std::string>(imageFile);
	cv::Mat imgMat = cv::imread(imageFileCvt, 0);
	
	//cv::imshow(imageFileCvt, imgMat);
	//cv::waitKey(0);

	if (_edgeBasedDetector == NULL)
	{
		return gcnew array<DuneSegment^>(0);
	}

	std::vector<dune::DuneSegment> segments = _edgeBasedDetector->Extract(imgMat);

	array<DuneSegment^>^ output = gcnew array<DuneSegment^>((int)segments.size());
	for (size_t i = 0; i < segments.size(); ++i)
	{
		std::vector<dune::DuneSegmentData> data = segments[i].GetSegmentData();
		output[i] = gcnew DuneSegment();
		output[i]->Points = gcnew array<Point2D^>((int)data.size());
		for (size_t j = 0; j < data.size(); ++j)
		{
			output[i]->Points[j] = gcnew Point2D();
			output[i]->Points[j]->x = data[j].Position.x;
			output[i]->Points[j]->y = data[j].Position.y;
		}
	}

	return output;
}

void DuneDetector::SetParameters(double R, int minSegmentLength, int K, double edgeThreshold, int domBins)
{
	_edgeDetectorParams->R = R;
	_edgeDetectorParams->MinSegmentLength = minSegmentLength;
	_edgeDetectorParams->ImageProcessParameters = new dune::EdgeDetectorProcParams(K, edgeThreshold, domBins);
	_edgeBasedDetector->SetParameters(_edgeDetectorParams);
}