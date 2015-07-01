#include "Examples.h"

#include <crtdbg.h>
using namespace stdext;
using namespace TVF;
using namespace TVF::Math;
using namespace TVF::DataStructures;
using namespace TVF::DataStructures::Spaces;
using namespace TVF::DataStructures::Spaces::Filters;
using namespace TVF::ProcessTools;
using namespace TVF::ProcessTools::Visitors;
using namespace TVF::Extensions::Curvature;
using namespace TVF::Extensions::Labelling;
using namespace TVF::ProcessTools::Marching;

void RunExample2()
{
	///////////////////////
	// DATA DECLARATION: //
	///////////////////////

	// The data values:
	vector_dim_t dim = 2, N = 100, i;
	float *data;
	bool res;
	// Allocating:
	data = new float[N*dim];

	std::vector<float> data_vec;
	std::vector<float> saliency_vec;

	// Computing positions:
	for (i = 0; i < N; i += 2) {
		// Using the angle:
		data[i] = i;
		data_vec.push_back(data[i]);
		data[i + 1] = 1.0f;
		data_vec.push_back(data[i + 1]);
		data[i+N] = i;
		data_vec.push_back(data[i+N]);
		data[i+N + 1] = -1.0f;
		data_vec.push_back(data[i+N + 1]);
	}

	/////////////////////
	// INITIALIZATION: //
	/////////////////////

	// Creating a set of parameters:
	float rad = 1.0f;
	TVParameters<float>::getInstance()
		->setParameter(TVPAR_RADIUS, rad)
		->setParameter(TVPAR_SCALE, rad / 4)
		->setParameterInt(TVPARI_DIM, dim)
		->setFlag(TVPARF_NORMALIZE, true)
		->setFlag(TVPARF_CURVATURE_USE, true)
		->setFlag(TVPARF_T2MASK, true)	// Only the stick tensor used:
		->getParameterMask(TVPARM_T2MASK)
		->setAll(false)
		->setBit(1, true);

	// Creating an appropriate factory:
	TVFactory<float> *fac = createTVFacProtoPredef
		<float, TVRETypesT1T2CompCurvature<float>, TVSpVector<float> >();

	// The process:
	TVProcess<float> *pro =
		new TVProSequential<float>(data, N, fac,
		new TVRecPlain<float>(2));

	/////////////////////////////////
	// TESTING THE VOTING PROCESS: //
	/////////////////////////////////

	// Local results:
	float *sals;
	int snum;

	// Using the recipe:
	pro->vote();

	// Obtaining the saliency values:
	TVParameters<float>::getInstance()
		->setFlag(TVPARF_CURVATURE_SALS, true);
	sals = pro->computeSaliency();
	snum = pro->computeTotalSalsNum();

	// Checking:
	for (res = true, i = snum - 1; i<snum*N; i += snum) {
		// Checking the value:
		//res = res && TEST_COMPARE_REALS(sals[i], 1.0f);
		saliency_vec.push_back(sals[i]);
	}
	//TEST_ASSERT("Unit circle curvature's must be 1!", res);
	delete sals;

	//TVVectorIterator<float> *titer = new TVVectorIterator<float>(data, N, dim);
	//TVObject<float> *objs = pro->voteTarget(titer);

	/*std::vector<float> votes;
	for (int i = 0; i < objs->getDim(); ++i)
	{
		votes.push_back(objs[i]);
	}

	delete titer;*/

	return;

}