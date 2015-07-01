
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

#ifndef _TV_TEMPLATE_TOOLS_H_
#define _TV_TEMPLATE_TOOLS_H_

// My namespace:
namespace TVF {

	/*
	*  Tools useful in the autoconfiguration of programs, implemented as
	* recursive template code.
	*/

	/*
	 * A types list implementation:
	 */
	struct Nil {};
	template <class CAR,class CDR=Nil>
	struct TVTLCons {
		// The head and tail:
		typedef CAR car;
		typedef CDR cdr;
	};

	// Measuring the length of a list:
	template <class List>
	struct TVTLLength {
		// The result:
		enum {RES = TVTLLength<List::cdr>::RES + 1};
	};
	template <>
	struct TVTLLength<Nil> {
		// The result:
		enum {RES = 0};
	};

	// Traversing the list (no check for shorter list):
	template <class List, int ind>
	struct TVTLListGet {
		// This is not the right one, getting the next:
		typedef typename TVTLListGet<typename List::cdr,ind-1>::RET RET;
	};
	template <class List>
	struct TVTLListGet<List,1> {
		// This is not the right one, getting the next:
		typedef typename List::car RET;
	};

} // namespace TVF

#endif //ndef _TV_TEMPLATE_TOOLS_H_
