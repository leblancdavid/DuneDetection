
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

#ifndef _MATHEXT_H_
#define _MATHEXT_H_

/*
 *  This library provides a set of extensions to the math.h
 * functions and the math.h library itself.
 */

// Useful libs:
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

// Useful constants:
#define EPS 1e-12
#ifndef M_PI
# define M_PI 3.14159265358979
#endif
#define M_SQR2PI 2.506628274631

////////////
// Macros //
////////////

// Min and max:
#ifndef MIN
# define MIN(__a,__b) ((__a)>(__b)?(__b):(__a))
#endif
#ifndef MAX
# define MAX(__a,__b) ((__a)<(__b)?(__b):(__a))
#endif
#ifndef ABS
# define ABS(__a) ((__a)>0? (__a): -(__a))
#endif

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		///////////////
		// Functions //
		///////////////

		// The mono-dimensional gaussian function:
		template <class T>
		inline T mext_gauss(T x,T sigma=1,T mu=0,bool norm=false) {
			// The return value:
			T ret;

			// Computing the gaussian:
			ret = x-mu;
			ret = exp(-(ret*ret)/(sigma*sigma*2));

			// Normalizing:
			if (norm) {
				ret /= sigma*(T)M_SQR2PI;
			}

			// Returning:
			return ret;
		}

	} // namespace Math
} // namespace TVF

#endif //ndef _MATHEXT_H_
