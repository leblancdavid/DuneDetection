
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

#ifndef _TVDFTENSOR2_H_
#define _TVDFTENSOR2_H_

// Includes:
#include <stdarg.h>

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		/*
		 *  This abstract decay function allows to manage automatically the
		 * varargs used in the second order tensors.
		 */

		template <class T>
		class TVDFTensor2 : public TVDFGaussian<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The set of precomputed decay values:
			T *_decays;

			// The old dimensionality:
			tensor_dim_t _dim;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVDFTensor2(T sigma=-1) : TVDFGaussian<T>(sigma), _decays(NULL), _dim(0) {}

			// Destructor:
			~TVDFTensor2() {
				// Deallocating the decays:
				vec_free(_decays);
			}

			/**********************
			 * Decay computation: *
			 **********************/

			// Decay computation:
			virtual T compute(const T *p,tensor_dim_t dim,...) {
				// Getting the base tensor order:
				tensor_dim_t ord;
				va_list ap;
				va_start(ap,dim);
				ord = va_arg(ap, tensor_dim_t);
				va_end(ap);

				// Checking the allocation of _decays:
				if (_dim!=dim || _decays==NULL) {
					// Unallocating:
					vec_free(_decays);

					// Reallocating:
					_decays = vec_create<T>(dim);
				}
				if (_dim!=dim) _dim = dim;

				// Computing and returning the decay:
				ord = ord % dim;	// Safety check
				return _decays[ord] = _compute(p,dim,ord);
			}

			/**************
			 * Accessors: *
			 **************/

			// Getter for the decays:
			const T *getDecays() const {
				// Returning:
				return _decays;
			}

		protected:
			// The real (abstract) decay function:
			virtual T _compute(const T *p,tensor_dim_t dim,tensor_dim_t ord)=0;
		};

	} // namespace Math
} // namespace TVF


#endif //ndef _TVDFTENSOR2_H_
