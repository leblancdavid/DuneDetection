
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

#ifndef _TVDFCOMP_H_
#define _TVDFCOMP_H_

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		/*
		 *  This decay function allows to use the space compression concept to compute
		 * the decay of second order tensors.
		 */

		// The decay function for the second order tensors:
		template<class T>
		class TVDFComp : public TVDFTensor2<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The standard deviation:
			T _sigma_squared;

			// The parameters:
			T _ha;
			T _in;
			T _per;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVDFComp(T sigma=-1) : TVDFTensor2<T>(sigma), _ha((T)0.6), _in((T)1), _per((T)2) {
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
					// Local values:
					T compr, ph[2], norm;

					// Computing the uncompressed and compressed parts:
					ph[0] = vec_norm_L2(p,dim-ord);
					ph[1] = vec_norm_L2(p+dim-ord,ord);

					// Computing the compression:
					norm = vec_norm_L2_squared(ph,2);
					compr = 1 + _in*pow(ABS(ph[1] / 
						(_sigma*pow(norm/_sigma_squared,_ha)+(T)EPS)),_per);

					// Computing the x squared value for the gaussian:
					compr = compr*compr*norm;

					// Returning:
					return exp(-compr/(2*_sigma_squared));
				}
			}

		public:
			/**************
			 * Accessors: *
			 **************/

			// Getter and setter for the sigma value:
			TVDFComp<T> *setSigma(const T sigma=1) {
				// Setting:
				TVDFGaussian<T>::setSigma(sigma);
				_sigma_squared = _sigma*_sigma;

				// Returning:
				return this;
			}

			// Getter and setter for the ha:
			T getHa() const {
				// Returning:
				return _ha;
			}
			TVDFComp<T> *setHa(const T ha=0.6) {
				// Setting:
				_ha = ha;

				// Returning:
				return this;
			}

			// Getter and setter for the in:
			T getIn() const {
				// Returning:
				return _in;
			}
			TVDFComp<T> *setIn(const T in=5) {
				// Setting:
				_in = in;

				// Returning:
				return this;
			}

			// Getter and setter for the ha:
			T getPer() const {
				// Returning:
				return _per;
			}
			TVDFComp<T> *setPer(const T per=2) {
				// Setting:
				_per = per;

				// Returning:
				return this;
			}
		};


	} // namespace Math
} // namespace TVF

#endif //ndef _TVDFCOMP_H_
