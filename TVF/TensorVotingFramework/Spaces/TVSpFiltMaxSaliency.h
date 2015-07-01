
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

#ifndef _TVSPFILTMAXSALIENCY_H_
#define _TVSPFILTMAXSALIENCY_H_

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
				class TVSpFiltMaxSaliency : public TVSpFilter<T> {
				protected:
					/***************
					* Attributes: *
					***************/

					// The threshold value:
					T _th;

					// The index of the remitter to be used:
					vector_dim_t _remInd;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					// Default constructor:
					TVSpFiltMaxSaliency(T th,TVSpFilter<T> *next=NULL,vector_dim_t remInd=1) : 
					TVSpFilter<T>(next), _th(th), _remInd(remInd) {}

					/*************************************
					* Abstract methods' implementation: *
					*************************************/

					// The predicate to be true for mantained objects:
					bool predicate(TVObject<T> *obj) const {
						// Getting the remitter:
						TVRemitter<T> *rem = obj->getRemitter(_remInd);

						// Getting the saliency values:
						T *sals = rem->computeSals();

						// Finding the max value;
						T maxVal = sals[vec_max(sals,rem->getSaliencyNum())];

						// Freeing memory:
						vec_free(sals);

						// Returning the threshold's result:
						return maxVal>=_th;
					}

					/**************
					* Accessors: *
					**************/

					// Getter and setter for the threshold:
					T getThreshold() const {
						// Returning:
						return _th;
					}
					void setThreshold(T th) {
						// Setting:
						_th = th;
					}
				};

			} // namespace Filters
		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPFILTMAXSALIENCY_H_
