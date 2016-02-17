#include "EdgeBasedDuneDetector.h"
#include "DuneSegment.h"

public ref class DuneDetector
{
public:
	DuneDetector() 
	{
		_edgeDetectorParams = new dune::EdgeBasedDetectorParameters();
		_edgeBasedDetector = new dune::EdgeBasedDuneDetector(new dune::EdgeDetectorImageProcessor(),
			_edgeDetectorParams);
	}
	~DuneDetector() { this->!DuneDetector(); }
	!DuneDetector() 
	{
	}

	array<DuneSegment^>^ GetDuneSegments(System::String^ imageFile);

	void SetParameters(double R, int minSegmentLength, int K, double edgeThreshold, int domBins);

private:
	dune::EdgeBasedDetectorParameters *_edgeDetectorParams;
	dune::EdgeBasedDuneDetector *_edgeBasedDetector;
};