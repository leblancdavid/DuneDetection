
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

#ifndef _TVSPFILTNUMVOTES_H_
#define _TVSPFILTNUMVOTES_H_

/*
 *  This filter allows to remove the objects where the max saliency returned
 * from a remitter is too small. To filter an object the filter-defined remitter
 * is retrieved the saliency values are required and the maximum value is found,
 * then a simple threshold on this value is used.
 */

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {
		namespace Spaces {
			namespace Filters {

				template <class T>
				class TVSpFiltNumVotes : public TVSpFilter<T> {
				protected:
					/***************
					* Attributes: *
					***************/

					// The threshold value:
					vector_dim_t _th;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					// Default constructor:
					TVSpFiltNumVotes(vector_dim_t th,TVSpFilter<T> *next=NULL) : 
					TVSpFilter<T>(next), _th(th) {}

					/*************************************
					* Abstract methods' implementation: *
					*************************************/

					// The predicate to be true for mantained objects:
					bool predicate(TVObject<T> *obj) const {
						// Returning the threshold's result:
						return obj->getNvotes()>=_th;
					}

					/**************
					* Accessors: *
					**************/

					// Getter and setter for the threshold:
					vector_dim_t getThreshold() const {
						// Returning:
						return _th;
					}
					void setThreshold(vector_dim_t th) {
						// Setting:
						_th = th;
					}
				};

			} // namespace Filters
		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPFILTNUMVOTES_H_
