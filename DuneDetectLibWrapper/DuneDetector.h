#include "EdgeBasedDuneDetector.h"
#include "DuneSegment.h"

public ref class DuneDetector
{
public:
	DuneDetector() { _edgeBasedDetector = new dune::EdgeBasedDuneDetector(); }
	~DuneDetector() { this->!DuneDetector(); }
	!DuneDetector() { delete _edgeBasedDetector; }

	array<DuneSegment^>^ GetDuneSegments(System::String^ imageFile);

private:
	dune::EdgeBasedDuneDetector *_edgeBasedDetector;
};