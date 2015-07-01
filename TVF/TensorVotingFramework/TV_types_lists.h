
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

#ifndef _TV_TYPES_LISTS_H_
#define _TV_TYPES_LISTS_H_

// The namespace:
namespace TVF {

	/*
	 *  Here the declaration of the types lists for remitters and decays:
	 */

	// Creating a templated types list:
	template <class T,class RemT2,class DecayT2>
	struct TVRETypesT1T2 {
		// The list of remitters:
		typedef typename TVTLCons<TVRETensor1<T>,TVTLCons<RemT2> > Remitters;
		// The list of decay functions:
		typedef typename TVTLCons<DecayT2,TVTLCons<DecayT2 > > Decays;
	};

	// Creating a set of default types lists with T1 and T2 tensorial remitters:
	template <class T>
	struct TVRETypesT1T2Comp : 
		public TVRETypesT1T2<T,TVRETensor2<T>,TVDFComp<T> > {};

	//  Creating a set of types lists with T1 and T2 tensorial remitters 
	// and original decays:
	template <class T>
	struct TVRETypesT1T2Original : 
		public TVRETypesT1T2<T,TVRETensor2<T>,TVDFOriginal<T> > {};

} // namespace TVF

#endif //ndef _TV_TYPES_LISTS_H_
