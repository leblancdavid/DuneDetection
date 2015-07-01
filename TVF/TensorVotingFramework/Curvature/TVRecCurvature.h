
/*
 * Copyright 2008 Gabriele Lombardi
 * 
 *
 *   This file is part of OpenTVF.
 *
 *   OpenTVF is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   OpenTVF is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with OpenTVF.  If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef _TVRECCURVATURE_H_
#define _TVRECCURVATURE_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Curvature {

			// Using the math namespace:
			using namespace TVF::Math;
			using namespace TVF::DataStructures;
			using namespace TVF::ProcessTools;

			/*
			 *  This recipe allows to compute the curvature on the object points and must
			 * be used with TVRETypesT1T2CompCurvature or TVRETypesT1T2OriginalCurvature.
			 */
			template <class T>
			class TVRecCurvature : public ITVRecipe<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// Number of iterations:
				short _iters;

				// The curvature must be used unce it was computed?
				bool _useCurvature;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVRecCurvature(short iters=2) : _iters(iters) {
					// Parameters:
					TVParameters<T> *params =
						TVParameters<T>::getInstance();

					// Must the curvature be used once it is estimated?
					_useCurvature = params->getFlag(TVPARF_CURVATURE_USE);
				}

				/*****************
				 * Recipe usage: *
				 *****************/

				// Running the recipe:
				virtual void run(TVProcess<T> *process) {
					// Parameters:
					TVParameters<T> *params =
						TVParameters<T>::getInstance();

					// Saving the bitmask:
					bool T2masked = params->getFlag(TVPARF_T2MASK);
					TVBitMask _bm(params->getParameterMask(TVPARM_T2MASK)->getSize());
					_bm = *(params->getParameterMask(TVPARM_T2MASK));

					// The first voting is executed with T1 enabled:
					params
						->setFlag(TVPARF_CURVATURE_USE,false)		// Do not use the curvature (must be computed firstly):
						->setFlag(TVPARF_CURVATURE_COMP,false)		// Do not compute the curvature during the first voting pass:
						->setFlag(TVPARF_T2MASK,true)				// Masking some T2 base tensros' votes:
						->getParameterMask(TVPARM_T2MASK)
							->setAll(false)							// All tensors masked:
							->setBit(0,true);						// Ball tensor voting only used to obtain orientations:

					// A first voting pass allows to compute the T1 tensors and estimate the T2 tensors:
					process->votingPass();

					// The other voting passes are executed with this options:
					params
						->setFlag(TVPARF_T2MASK,T2masked)			// Mask if it was required:
						->setFlag(TVPARF_CURVATURE_COMP,true);		// Now the curvature can be computed:

					// Set the old mask:
					*(params->getParameterMask(TVPARM_T2MASK)) = _bm;

					// Using the actually set parameters:
					for (short i=1; i<_iters; i++) {
						// Other parameters for i>1:
						if (i==2) {
							// Setting the parameters:
							params
								->setFlag(TVPARF_CURVATURE_COMP,false)			// Do not re-compute the curvature:
								->setFlag(TVPARF_CURVATURE_USE,_useCurvature);	// If required use the curvature:
						}

						// A voting pass:
						process->votingPass();
					}
				}

				/**************
				 * Accessors: *
				 **************/

				// Setter and getter for the iterations number:
				TVRecCurvature<T> *setIters(short iters=2) {
					// Setting:
					_iters = iters;

					// Returning:
					return this;
				}
				short getIters() const {
					// Returning:
					return _iters;
				}

				// Setter and getter for the usage of curvature:
				TVRecCurvature<T> *setUseCurvature(bool useCurvature=true) {
					// Setting:
					_useCurvature = useCurvature;

					// Returning:
					return this;
				}
				bool getUseCurvature() const {
					// Returning:
					return _useCurvature;
				}
			};

		} // namespace Curvature
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVRECCURVATURECOMPUTE_H_
