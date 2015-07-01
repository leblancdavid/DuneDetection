
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

#ifndef _TVDFCURVATUREBASE_H_
#define _TVDFCURVATUREBASE_H_

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
			template<class T>
			class TVDFCurvatureBase : public TVDFTensor2<T> {
			public:
				/**************
				* Accessors: *
				**************/

				/*
				*  This accessors allows to manage the mean sectional cuvature vector.
				* The managed pointer must boint to a correctly sized vector, no checks
				* are done here.
				*/
				virtual void setDir(const T *T1)=0;

				// Getter for the computed decays and curvature:
				virtual const T *getCurvature() const=0;

				// The uncurved decay function:
				virtual ITVDecayFun<T> *getInternalDecayFunction()=0;
			};

		} // namespace Curvature
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVDFCURVATUREBASE_H_
