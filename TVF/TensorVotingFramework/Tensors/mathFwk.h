
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

#ifndef _MATHFWK_H_
#define _MATHFWK_H_

//////////
// Libs //
//////////

// The tensor header includes also the matrix one:
#include "mathTensors.h"


///////////
// Funcs //
///////////

// The mathematical stuff is in the TVF::Math namespace:
namespace TVF {
	namespace Math {

		// Cleaning the framework:
		template<class T>
		void mathFwk_free_static_buffs() {
			// Cleaning:
			mat_free_static_buffs<T>();
			tns_free_static_buffs<T>();
		}

	} // namespace Math
} // namespace TVF

#endif //ndef _MATHFWK_H_
