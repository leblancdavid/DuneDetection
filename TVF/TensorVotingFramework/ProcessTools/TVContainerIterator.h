
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

#ifndef _TVCONTAINER_H_
#define _TVCONTAINER_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		// A range iterator:
		template <class CONTAINER,class ELEMENT>
		class TVContainerIterator : public ITVIterator<ELEMENT> {
		protected:
			/**********
			 * Types: *
			 **********/

			// The container's type:
			typedef CONTAINER _container_t;

			// The container's iterator:
			typedef typename _container_t::iterator _iterator_t;

			/***************
			 * Attributes: *
			 ***************/

			// The container:
			_container_t *_container;

			// The iterator:
			_iterator_t _i;

			// Has the actual element?:
			bool _hasActual;

			// Is the container aggregated?
			bool _isAggregated;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVContainerIterator(_container_t *container,bool isAggregated=false) : 
					_container(container), _isAggregated(isAggregated) {
				// Resetting:
				reset();
			}

			// Destructor:
			~TVContainerIterator() {
				// Eliminating the container:
				if (!_isAggregated) {
					// Deleting:
					delete _container;
				}
			}

			/*******************
			 * Moving methods: *
			 *******************/

			// Resetting the iterator (restarts from the first element):
			virtual void reset() {
				// Getting the begin iterator:
				_i = _container->begin();
				if (_i!=_container->end()) {
					// Has an actual element:
					_hasActual = true;
				}
			}

			// Moving forward:
			virtual void next() {
				// Next element:
				_i++;
				if (_i==_container->end()) {
					// No actual element:
					_hasActual = false;
				}
			}

			/********************
			 * Getting methods: *
			 ********************/

			// Getting the actual element:
			virtual ELEMENT getActual() const {
				// Returning:
				if (hasActual()) {
					// The pointed element:
					return *_i;
				}
				else {
					// NULL:
					return (ELEMENT)NULL;
				}
			}

			/********************
			 * Quering methods: *
			 ********************/

			// Ask for the presence of the actual element:
			virtual bool hasActual() const {
				// Returning:
				return _hasActual;
			}

			// Getting the number of elements:
			virtual vector_ind_t getNumel() {
				// Returning:
				return (vector_ind_t)_container->size();
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVCONTAINER_H_
