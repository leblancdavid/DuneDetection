
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

#ifndef _TVDFGAUSSIAN_H_
#define _TVDFGAUSSIAN_H_

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		/*
		 *  This decay function allows to use the gaussian unnormalized function to compute
		 * the decay of first order tensors.
		 */

		// The decay function for the first order tensors:
		template<class T>
		class TVDFGaussian : public ITVDecayFun<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The standard deviation:
			T _sigma;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVDFGaussian(T sigma=-1) : _sigma(sigma) {
				// Setting the default:
				if (sigma<=0) {
					// Getting it from the parameters:
					_sigma = TVParameters<T>::getInstance()
						->getParameter(TVPAR_SCALE);
				}
			}

			/**********************
			 * Decay computation: *
			 **********************/

			// Decay computation:
			virtual T compute(const T *p,tensor_dim_t dim,...) {
				// The saliency is the gaussian function computed in the given point:
				return mext_gauss(vec_norm_L2(p,dim),_sigma);
			}

			/**************
			 * Accessors: *
			 **************/

			// Getter and setter for the sigma value:
			T getSigma() const {
				// Returning:
				return _sigma;
			}
			TVDFGaussian<T> *setSigma(const T sigma=1) {
				// Setting:
				_sigma = sigma;

				// Returning:
				return this;
			}
		};

	} // namespace Math
} // namespace TVF

#endif //ndef _TVDFGAUSSIAN_H_
