
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

#ifndef _ITVITERATOR_H_
#define _ITVITERATOR_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	 *  This is a genreal iterator that can be used for
	 * every type given as template argument.
	 */

	template <class ELEMENT>
	class ITVIterator {
	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// Destructor:
		virtual ~ITVIterator() {}

		/*******************
		 * Moving methods: *
		 *******************/

		// Resetting the iterator (restarts from the first element):
		virtual void reset()=0;

		// Moving forward:
		virtual void next()=0;

		/********************
		 * Getting methods: *
		 ********************/

		// Getting the actual element:
		virtual ELEMENT getActual() const=0;

		/********************
		 * Quering methods: *
		 ********************/

		// Ask for the presence of the next element:
		virtual bool hasActual() const=0;

		// Getting the number of elements:
		virtual vector_ind_t getNumel() {
			// By default the iteration is used:
			vector_ind_t N = 0;
			for (reset(); hasActual(); next()) N++;

			// Returning:
			return N;
		}
	};

} // namespace TVF

#endif //ndef _ITVITERATOR_H_
