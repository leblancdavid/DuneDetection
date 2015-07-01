
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

#ifndef _TVSPFILTER_H_
#define _TVSPFILTER_H_

/*
 *  This abstract class defines a filter over a space: it allows to
 * filter a space with a predicate. The usage is really simple: the
 * abstract method "predicate" receives an object and decides if it
 * must be mantained or not (with a boolean value that must be true
 * for objects to be manained). To generate a filter sublcass this
 * abstract class and implement the predicate method. The static
 * method "filter" can than be used to filter a space.
 */

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {
		namespace Spaces {
			namespace Filters {

				template <class T>
				class TVSpFilter {
				protected:
					/***************
					* Attributes: *
					***************/

					// The next filter of the chain:
					TVSpFilter<T> *_next;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					// Constructor:
					TVSpFilter(TVSpFilter<T> *next=NULL) : _next(next) {}

					// Empty destructor:
					virtual ~TVSpFilter() {
						// Deleting the filter chain:
						if (_next) delete _next;
					}

					/*********************
					* Abstract methods: *
					*********************/

					// The predicate to be true for mantained objects:
					virtual bool predicate(TVObject<T> *obj) const=0;

					/**************
					* Accessors: *
					**************/

					// Getter and setter for the next filter:
					TVSpFilter<T> *getNextFilter() const {
						// Returning:
						return _next;
					}
					TVSpFilter<T> *detachNextFilter() const {
						// Removing:
						TVSpFilter<T> *res = _next;
						_next = NULL;

						// Returning:
						return res;
					}
					void setNextFilter(TVSpFilter<T> *next) {
						// Setting:
						if (_next) delete _next;
						_next = next;
					}

					/*******************
					* Filtering tool: *
					*******************/

					// The filtering method:
					static void filter(TVSpace<T> *sp,TVSpFilter<T> *filter) {
						// I have to use a pointer mask here:
						TVObject<T> **toBeRemoved = new TVObject<T>*[sp->getSize()];
						
						// Indexes and iterators:
						vector_ind_t i,j;
						TVObject<T>::iterator_t *iter = sp->getIterator();
						TVObject<T> *obj;

						// Iterating on filters:
						for (; filter!=NULL; filter=filter->getNextFilter()) {
							// Collecting the objects to be removed:
							for (i=0, iter->reset(); iter->hasActual(); iter->next()) {
								// Checking the predicate:
								obj = iter->getActual();
								if (!filter->predicate(obj)) {
									// Must be removed:
									toBeRemoved[i++] = obj;
								}
							}

							// Removing objects:
							for (j=0; j<i; j++) {
								// Removing an object:
								sp->remove(toBeRemoved[j]);
							}
						}

						// Freeing memory:
						delete iter;
						delete toBeRemoved;
					}
				};

			} // namespace Filters
		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPFILTER_H_
