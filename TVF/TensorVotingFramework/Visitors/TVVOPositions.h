
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

#ifndef _TVVOPOSITIONS_H_
#define	 _TVVOPOSITIONS_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Visitors {

			/*
			 * This class allows to save objects' positions in a matrix.
			 */

			template <class T>
			class TVVOPositions : public ITVVisitObject<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The destination buffer:
				T *_dest;

				// The actual position:
				T *_pos;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVVOPositions(T *dest) : _dest(dest) {}

				// Destructor:
				virtual ~TVVOPositions() {}

				/************************************
				* Implementation of the interface: *
				************************************/

				// Initialization:
				virtual void init() {
					// The position:
					_pos = _dest;
				}

				// The visit method:
				virtual void visit(TVObject<T> *obj) {
					// The dimensionality:
					vector_dim_t dim = obj->getDim();

					// Saving the point:
					vec_create_clone(_pos,obj->getPos(),dim);

					// Next:
					_pos += dim;
				}

				/**************
				* Accessors: *
				**************/

				// Getter for the buffer:
				T *getDestinationBuffer() const {
					// Returning:
					return _dest;
				}
			};

		} // namespace Visitors
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVVOPOSITIONS_H_
