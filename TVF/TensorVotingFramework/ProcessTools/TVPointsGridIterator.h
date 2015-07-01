
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

#ifndef _TVPOINTSGRIDITERATOR_H_
#define _TVPOINTSGRIDITERATOR_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		/*
		 *  This iterator allows to iterate on a regular grid of points in a
		 * D-dimensional space given the starting point, the ending point and
		 * the steps.
		 * NOTICE:	 The given vecotrs (start, end and steps) are not unallocated,
		 *			whilst the point is!
		 */

		template <class T>
		class TVPointsGridIterator : public ITVIterator<T*> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The starting point:
			T *_start;

			// The ending point:
			T *_end;

			// The steps:
			T *_steps;

			// Dimensionality:
			vector_ind_t _dim;

			// Actual element:
			T *_point;

			// Is the end reached?
			bool _endReached;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVPointsGridIterator(T *start,T *end,T *steps,vector_ind_t dim) :
					_start(start), _end(end), _dim(dim), _steps(steps) {
				// Allocating the point:
				_point = new T[dim];

				// Resetting:
				reset();
			}

			// Destructor:
			~TVPointsGridIterator() {
				// Deleting the point:
				delete _point;
			}

			/*******************
			 * Moving methods: *
			 *******************/

			// Resetting the iterator (restarts from the first element):
			virtual void reset() {
				// Starting from the starting point:
				memcpy(_point,_start,_dim*sizeof(T));
				_endReached = false;
			}

			// Moving forward:
			virtual void next() {
				// Check for the end:
				if (_endReached) return;

				// Modifying the "digits":
				for (vector_ind_t i=0; i<_dim; i++) {
					// Modifying a value:
					_point[i] += _steps[i];

					// Checking:
					if (i<_dim-1 && _point[i]>_end[i]+_steps[i]/2) {
						// This dimension must restart from zero:
						_point[i] = _start[i];
					}
					else {
						// Done:
						break;
					}
				}

				// GOOD, reaching this code means that modifications are finished!
				if (_point[_dim-1]>_end[_dim-1]+_steps[_dim-1]/2) {
					// End of points:
					_endReached = true;
				}
			}

			/********************
			 * Getting methods: *
			 ********************/

			// Getting the actual element:
			virtual T *getActual() const {
				// Checking:
				if (hasActual()) {
					// Returning:
					return _point;
				}
				else {
					// End of points:
					return NULL;
				}
			}

			/********************
			 * Quering methods: *
			 ********************/

			// Ask for the presence of the next element:
			virtual bool hasActual() const {
				// It depends on pos:
				return !_endReached;
			}

			// Getting the number of elements:
			virtual vector_ind_t getNumel() {
				// The counter:
				vector_ind_t N = 1,i;

				// Iterating on dimensions:
				for (i=0; i<_dim; i++) {
					// Multiplying for the number of bins along this direction:
					N *= (vector_ind_t)((_end[i]-_start[i])/_steps[i]+(T)1.5);
				}

				// Returning:
				return N;
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVPOINTSGRIDITERATOR_H_
