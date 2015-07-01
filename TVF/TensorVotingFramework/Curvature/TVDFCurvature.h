
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

#ifndef _TVDFCURVATURE_H_
#define _TVDFCURVATURE_H_

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Extensions {
		namespace Curvature {

			// Using the math namespace:
			using namespace TVF::Math;

			/*
			*  This decay function allows to use the gaussian unnormalized function to compute
			* the decay of first order tensors.
			*/

			// The decay function for the curvature usage:
			template<class T,class DF>
			class TVDFCurvature : public TVDFCurvatureBase<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The uncurved decay function:
				DF *_decayFunction;

				// The curvatures' vector (composition):
				T *_curvatures;

				// The directions' vectors (oriented as the T2 tensor):
				const T *_dir;

				// The temporary vectors used for the new point position computation and storage:
				T *_tmp_pos, *_dirHat;

				// The dimensionality of the curvatures vector:
				tensor_dim_t _dim;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVDFCurvature(T sigma=-1) : 
				_curvatures(NULL), _dim(0), _dir(NULL), _tmp_pos(NULL), _dirHat(NULL) {
					// Allocating the decay function:
					_decayFunction = new DF(sigma);
				}

				// Destructor:
				~TVDFCurvature() {
					// Deleting the unused memory:
					vec_free(_curvatures);
					vec_free(_tmp_pos);
					vec_free(_dirHat);
					delete _decayFunction;
				}

			protected:
				/**********************
				* Decay computation: *
				**********************/

				// Decay computation:
				virtual T _compute(const T *p,tensor_dim_t dim,tensor_dim_t ord) {
					// Checking the vectors space:
					if (!_curvatures || dim!=_dim) {
						// Deallocating and reallocating:
						vec_free(_curvatures);
						_curvatures = vec_create<T>(dim-1);
					}
					if (!_tmp_pos || dim!=_dim) {
						// Deallocating and reallocating:
						vec_free(_tmp_pos);
						_tmp_pos = vec_create<T>(dim);
					}
					if (!_dirHat || dim!=_dim) {
						// Deallocating and reallocating:
						vec_free(_dirHat);
						_dirHat = vec_create<T>(dim);
					}
					if (dim!=_dim) _dim = dim;

					// Local vars:
					T decay;

					// Must the curvature be managed?
					if (ord>0) {
						// Local vars:
						T k, s, length, theta;

						// If required, the previously computed mean curvature and the dir vector are used:
						if (_dir) {
							// Local vars:
							T X,Y,X2,Y2,R;

							// My direction:
							const T *myDir = _dir + TNS_DOF_NUM(ord-1);

							// The radius:
							R = 1/(vec_norm_L2(myDir,ord)+(T)EPS);

							// Computing the new point position:
							// - The T1hat vector (projection of dir on the compressed dimensions):
							vec_create_clone(_dirHat,myDir,ord);

							// - Normalizing dirHat:
							vec_normalize(_dirHat,ord,vec_norm_L2<T>);

							// - Computing X on the mapping plane (as norm of the projection on the uncompressed dimensions):
							X = vec_norm_L2<T>(p,dim-ord);

							// - Computing Y on the mapping plane (as projection on _dirHat):
							Y = vec_dot(p+(dim-ord),_dirHat,ord);

							// - The mapping:
							X2 = atan2(X,R-Y)*R;
							Y2 = sqrt(X*X+(Y-R)*(Y-R)) - R;

							// - Preparing the base vector:
							vec_create_clone(_tmp_pos,p,dim);

							// - The projection on the uncompressed dimensions:
							vec_scale<T>(_tmp_pos,(T)(X2/(X+EPS)),dim-ord);

							// - The projection on the compressed dimensions:
							vec_accumulate_scaled(_tmp_pos+(dim-ord),_dirHat,Y2-Y,ord);

							// - Computing the decay value on that point:
							decay = _decayFunction->compute(_tmp_pos,dim,ord);
						}
						else {
							// Using simply the decay function given:
							decay = _decayFunction->compute(p,dim,ord);
						}

						// Check if the curvature must be computed:
						if (TVParameters<T>::getInstance()->getFlag(TVPARF_CURVATURE_COMP)) {
							// Computing the actual curvature:
							vec_circ_curvature_and_length<T>(p,ord,dim,s,k,length,theta);

							// Saving the actual curvature:
							_curvatures[ord-1] = k;
						}
					}
					else {
						// Using simply the decay function given:
						decay = _decayFunction->compute(p,dim,ord);
					}

					// Saving and returning:
					return decay;
				}

				public:
				/**************
				* Accessors: *
				**************/

				/*
				*  This accessors allows to manage the mean sectional cuvature vector.
				* The managed pointer must boint to a correctly sized vector, no checks
				* are done here.
				*/
				void setDir(const T *dir) {
					// Setting without checks:
					_dir = dir;
				}

				// Getter for the computed curvature:
				const T *getCurvature() const {
					// Returning:
					return _curvatures;
				}

				// The uncurved decay function:
				ITVDecayFun<T> *getInternalDecayFunction() {
					// Returning:
					return _decayFunction;
				}
			};

		} // namespace Curvature
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVDFCURVATURE_H_
