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

void RunExample1()
{
	///////////////////////
	// DATA DECLARATION: //
	///////////////////////

	// The data values:
	vector_dim_t dim = 2, N = 100, i;
	float *angles;
	float *data;
	bool res;

	// Allocating:
	angles = new float[N];
	data = new float[N*dim];

	std::vector<float> angles_vec;
	std::vector<float> data_vec;
	std::vector<float> saliency_vec;

	// Computing angles:
	for (i = 0; i<N; i++) {
		// An angle:
		angles[i] = (float)(2 * (M_PI*i) / N);
		angles_vec.push_back(angles[i]);
	}

	// Computing positions:
	for (i = 0; i<2 * N; i += 2) {
		// Using the angle:
		data[i] = cos(angles[i / 2]);
		data_vec.push_back(data[i]);
		data[i + 1] = sin(angles[i / 2]);
		data_vec.push_back(data[i + 1]);
	}

	/////////////////////
	// INITIALIZATION: //
	/////////////////////

	// Creating a set of parameters:
	float rad = 4.0f;
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
		new TVRecCurvature<float>(3));

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

	// Doubling the circle:
	//for (i = 0; i<dim*N; i++) {
	//	// Modifying a point:
	//	data[i] *= 2;
	//	data_vec[i] *= 2.0f;
	//}

	//Added noise
	/*data[12] += 3.f;
	data[13] += 3.f;
	data_vec[12] += 3.f;
	data_vec[13] += 3.f;*/
	
	//does order matter?
	/*float tmpx = data[12], tmpy = data[13];
	data[12] = data[20];
	data[13] = data[21];
	data[20] = tmpx;
	data[21] = tmpy;*/

	//what about shifting some of the points
	for (int i = 0; i < N; i+= 2)
	{
		data[i] -= 2.0f;
	}


	// Resetting the tensors:
	pro->getSpace()->reset();

	// Voting again:
	TVParameters<float>::getInstance()
		->setParameter(TVPAR_RADIUS, rad * 4)
		->setParameter(TVPAR_SCALE, rad);
	pro->vote();

	// Obtaining the saliency values again:
	TVParameters<float>::getInstance()
		->setFlag(TVPARF_CURVATURE_SALS, true);
	sals = pro->computeSaliency();
	snum = pro->computeTotalSalsNum();

	// Checking again:
	for (res = true, i = snum - 1; i<snum*N; i += snum) {
		// Checking the value:
		res = res && abs(sals[i] - 0.5f)<1e-3;
		saliency_vec[i/snum] = sals[i];
	}

	return;
}