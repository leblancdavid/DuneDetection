
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

#ifndef _TVVECTORITERATOR_H_
#define _TVVECTORITERATOR_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		/*
		 *  This is a genreal iterator that can work on vectors of elements
		 * (arranged like in an unrolled matrix).
		 */

		template <class ELEMENT>
		class TVVectorIterator : public ITVIterator<ELEMENT*> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The vectors' vector:
			ELEMENT *_mat;

			// The position in the vector:
			ELEMENT *_pos;

			// Dimensionality:
			vector_ind_t _dim;

			// Number of elements:
			vector_ind_t _N;

			// Actual element:
			vector_ind_t _i;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVVectorIterator(ELEMENT *mat,vector_ind_t N,vector_ind_t dim) :
			_mat(mat), _pos(mat),_dim(dim), _N(N), _i(0) {}

			/*******************
			 * Moving methods: *
			 *******************/

			// Resetting the iterator (restarts from the first element):
			virtual void reset() {
				// Resetting the pointer:
				_pos = _mat;
				_i = 0;
			}

			// Moving forward:
			virtual void next() {
				// Checking:
				if (_pos) {
					// Moving:
					_pos += _dim;
					_i++;
					// Checking:
					if (_i>=_N) {
						// End of points:
						_pos = NULL;
					}
				}
			}

			/********************
			 * Getting methods: *
			 ********************/

			// Getting the actual element:
			virtual ELEMENT *getActual() const {
				// Checking:
				if (hasActual()) {
					// Returning:
					return _pos;
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
				return _pos!=NULL;
			}

			// Getting the number of elements:
			virtual vector_ind_t getNumel() {
				// Returning:
				return _N;
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVVECTORITERATOR_H_
