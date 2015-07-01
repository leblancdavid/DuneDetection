
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

#ifndef _TVFACTORY_H_
#define _TVFACTORY_H_

/*
 *  Here the base factory abstract class is defined.
 */

// All this in the TVF::ProcessTools namespace:
namespace TVF {
	namespace ProcessTools {

		// Namespaces used:
		using namespace TVF::Math;

		template <class T>
		class TVFactory {
		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Destructor:
			virtual ~TVFactory() {}

			/*********************
			 * Abstract methods: *
			 *********************/

			// Creation of an object:
			virtual TVObject<T> *createObject(
					T *coords			// The pointer to the position vector of the object.
				) const=0;

			// Creation of a space:
			virtual TVSpace<T> *createSpace(
					T *data,			// The matrix containing the positions of the objects.
					vector_ind_t N,		// The number of objects to be created in the space.
					vector_dim_t dim	// The dimensionality of the space.
				) const=0;
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVFACTORY_H_
