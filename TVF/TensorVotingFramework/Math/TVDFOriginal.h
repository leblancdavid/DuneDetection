
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

#ifndef _TVDFORIGINAL_H_
#define _TVDFORIGINAL_H_

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		/*
		 *  This decay function allows to use the gaussian unnormalized function to compute
		 * the decay of first order tensors.
		 */

		// The decay function for the second order tensors:
		template<class T>
		class TVDFOriginal : public TVDFTensor2<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The standard deviation:
			T _2sigma_squared;

			// The "magic" constant:
			static T _c;

			// Length threshold:
			static T _leps;
		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVDFOriginal(T sigma=-1) : TVDFTensor2<T>(sigma) {
				// Setting the sigma value:
				setSigma(_sigma);
			}

		protected:
			/**********************
			 * Decay computation: *
			 **********************/

			// Decay computation:
			virtual T _compute(const T *p,tensor_dim_t dim,tensor_dim_t ord) {
				// Checking the simple case:
				if (ord==0) {
					// The gaussian decay:
					return TVDFGaussian<T>::compute(p,dim);
				}
				else {
					// Curvature and arc length:
					T s,k,length,theta;

					// Curvature and arc length:
					vec_circ_curvature_and_length<T>(p,ord,dim,s,k,length,theta);

					// Zero length returns one:
					if (length<_leps && length>-_leps) {
						// Setting and returning:
						return (T)1;
					}

					// Bad theta returns zero:
					if ((theta>(T)M_PI/4 && theta<3*(T)M_PI/4) ||
						(theta<-(T)M_PI/4 && theta>-3*(T)M_PI/4)) {
						// Setting and returning:
						return (T)0;
					}

					// Computing and returning the result:
					return exp(-(s*s+_c*k*k)/_2sigma_squared);
				}
			}

		public:
			/**************
			 * Accessors: *
			 **************/

			// Getter and setter for the sigma value:
			TVDFOriginal<T> *setSigma(const T sigma=1) {
				// Setting:
				TVDFGaussian<T>::setSigma(sigma);
				_2sigma_squared = 2*_sigma*_sigma;
				_c = (T)3.6*_sigma;
				_leps = (T)1e-4*_sigma;

				// Returning:
				return this;
			}

			// Getting the constant used:
			static T getC() {
				// Returning:
				return _c;
			}
		};

		// Real declaration of the "magic" constant:
		template<class T>
		T TVDFOriginal<T>::_c = (T)3.6;

		// Real declaration of the length threshold:
		template<class T>
		T TVDFOriginal<T>::_leps = (T)1e-4;

	} // namespace Math
} // namespace TVF

#endif //ndef _TVDFORIGINAL_H_
