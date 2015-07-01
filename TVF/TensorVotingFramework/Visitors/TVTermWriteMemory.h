
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

#ifndef _TVTERMWRITEMEMORY_H_
#define _TVTERMWRITEMEMORY_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Visitors {

			/*
			*  This class represent a general visitor for the remitter hierarchy.
			*/
			template <class T>
			class TVTermWriteMemory : public ITVTerminal<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The buffer where the data must be saved:
				T *_buf;

				// Actual position:
				T *_pos;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVTermWriteMemory(T *buf=NULL) : 
					_buf(buf), _pos(buf) {}

				// Deleting all:
				~TVTermWriteMemory() {}

				/************************************
				* Implementation of the interface: *
				************************************/

				// Initialization:
				virtual void init() {
					// Initializing vars:
					_pos = _buf;
				}

				// Plot of the remitter:
				virtual void plotRemitter(TVRemitter<T> *rem) {
					// Checking:
					if (dynamic_cast<TVRETensor1<T> *>(rem)!=NULL) {
						// Calling:
						this->plotRemitter(dynamic_cast<TVRETensor1<T> *>(rem));
					}
					else if (dynamic_cast<TVRETensor2<T> *>(rem)!=NULL) {
						// Calling:
						this->plotRemitter(dynamic_cast<TVRETensor2<T> *>(rem));
					}
					// Ignoring other types of remitters.
				}

				// Plot of the T1 remitter:
				virtual void plotRemitter(TVRETensor1<T> *rem) {
					// Dimensionality:
					vector_dim_t dim = rem->getDim();

					// Saving:
					vec_create_clone(_pos,rem->getTensor(),dim);

					// Moving:
					_pos += dim;
				}

				// Plot of the T2 remitter:
				virtual void plotRemitter(TVRETensor2<T> *rem) {
					// Saving:
					vector_dim_t dim = rem->getDim();
					tns_copy_in_matrix(_pos,rem->getTensor(),dim);

					// Moving:
					_pos += dim*dim;
				}

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the buffer:
				T *getBuffer() const {
					// Returning:
					return _buf;
				}
				TVTermWriteMemory<T> *setBuffer(T *buf) {
					// Saving:
					_buf = buf;

					// Returning:
					return this;
				}
			};

		} // namespace Visitors
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVTERMWRITEMEMORY_H_
