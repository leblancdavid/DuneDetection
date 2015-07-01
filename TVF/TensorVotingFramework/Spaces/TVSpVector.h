
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

#ifndef _TVSPVECTOR_H_
#define _TVSPVECTOR_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {
		namespace Spaces {

			/*
			*  This class represents a space data structure simple and quick to
			* implement, this is the worst data structure from the time (and space?)
			* complexity point of view. The only purpose of this data structure is
			* to be used in testing and in Tensor Voting for only a few tensors.
			*/

			template <class T>
			class TVSpVector : public TVSpace<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The objects vector:
				TVObject<T> **_objs;

				// The array size:
				vector_ind_t _size;

				// The effective number of elements:
				vector_ind_t _numel;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// The default constructor:
				TVSpVector(vector_dim_t dim=0,vector_ind_t initialSpace=10)
						: TVSpace<T>(dim), _size(initialSpace), _numel(0) {
					// Generating the space vector:
					_objs = new TVObject<T>*[_size];
				}

				// Destructor:
				~TVSpVector() {
					//  The space is composed of objects, so it has the responsability
					// of the objects' deallocation:
					clean();

					// Removing the vector:
					delete _objs;
				}

				// Generating a new (empty) instance:
				virtual TVSpace<T> *newInstance(vector_dim_t dim=0) const {
					// Creating a new vector:
					if (dim==0) dim = _dim;
					return new TVSpVector<T>(dim);
				}

				/***************************
				* Structure manipulation: *
				***************************/

				// Getter for the number of objects:
				virtual vector_ind_t getSize() const {
					// Returning the number of elements:
					return _numel;
				}

				// Adding an object:
				virtual void add(TVObject<T> *obj) {
					// Checking the size:
					_checkSize(_numel+1);

					// Adding the object:
					_objs[_numel++] = obj;
				}

				// Extracting an object:
				virtual TVObject<T> *extract(TVObject<T> *obj) {
					// The index of the element:
					vector_ind_t i;

					// The object to be returned:
					TVObject<T> *o = NULL;

					// Checking the size:
					_checkSize(_numel-1);

					// Finding the element and selecting the case:
					if ((i=_lookFor(obj))!=_numel) {
						// Found.. removing:
						o = _objs[i];
						_numel--;

						// Saving the position:
						_objs[i] = _objs[_numel];
					}

					// Returning:
					return o;
				}

				// Updating an object when its position is externally changed:
				virtual void update(TVObject<T> *obj) {
					// None to be done in this case!
				}

				// Extracting all the objects:
				virtual void empty() {
					// The real number of elements:
					_numel = 0;
				}

				// Cleaning (removing the objects):
				virtual void clean() {
					// Deleting the objects:
					for (vector_ind_t i=0; i<_numel; i++) {
						// Deleting:
						delete _objs[i];
					}

					// The real number of elements:
					_numel = 0;
				}

				// Compactification (reducing the used memory):
				virtual void compact() {
					// Creating a vector of the correct size:
					TVObject<T> **objs = new TVObject<T>*[_numel];

					// Copy of the useful pointers:
					memcpy(objs,_objs,_numel*sizeof(TVObject<T>*));

					// Removing the old vector:
					delete _objs;

					// Substitution:
					_objs = objs;
					_size = _numel;
				}

				// Getting the iterator over all the objects:
				virtual typename TVObject<T>::iterator_t *getIterator() const {
					// Creating an iterator with me:
					return new TVSpVectorIterFull(this);
				}

				// Getting a range of objects:
				virtual typename TVObject<T>::iterator_t *range(const T *center, T radius=-1) const {
					// Managing the radius:
					if (radius<0) {
						// Getting the default one:
						radius = TVParameters<T>::getInstance()->getParameter(TVPAR_RADIUS);
					}

					// Creating an iterator with me:
					return new TVSpVectorIterRange(this,center,radius);
				}

				// Resetting all objects:
				virtual void reset() {
					// Resetting all the remitters:
					TVObject<float>::iterator_t *iter = getIterator();
					for (; iter->hasActual(); iter->next()) {
						// Resetting all:
						iter->getActual()->reset();
					}
					delete iter;
				}

			protected:
				/*************************
				* My protected methods: *
				*************************/

				// Checks the size of the vector and modifies it if required:
				void _checkSize(vector_ind_t required) {
					// Check if the size is sufficient:
					if (_size<required || _size>required*4) {
						// Reallocation must be done:
						_size = required*2 + 1;
						_size = _size<_numel? _numel: _size;
						TVObject<T> **objs = new TVObject<T>*[_size];

						// Copy of the useful pointers:
						memcpy(objs,_objs,_numel*sizeof(TVObject<T>*));

						// Removing the old vector:
						delete _objs;

						// Substitution:
						_objs = objs;
					}
				}

				//  Looking for an object (_numel is returned if the object
				// is not found, if it is found the index is returned):
				vector_ind_t _lookFor(TVObject<T> *obj) {
					// Iterating:
					vector_ind_t ret;
					for (ret=0; ret<_numel; ret++) {
						// Check:
						if (_objs[ret]==obj) return ret;
					}

					// Not found:
					return _numel;
				}

			protected:
				/********************************************************************
				*                            My iterators:                         *
				********************************************************************/

				// An iterator for all the elements:
				class TVSpVectorIterFull : public ITVIterator<TVObject<T>*> {
				protected:
					/***************
					* Attributes: *
					***************/

					// The space:
					const TVSpVector<T> *_sp;

					// The actual element:
					vector_ind_t _i;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					// Constructor:
					TVSpVectorIterFull(const TVSpVector<T> *sp) : _sp(sp), _i(0) {}

					/*******************
					* Moving methods: *
					*******************/

					// Resetting the iterator (restarts from the first element):
					virtual void reset() {
						// Restarting from 0:
						_i = 0;
					}

					// Moving forward:
					virtual void next() {
						// Moving forward:
						if (_i<_sp->_numel) _i++;
					}

					/********************
					* Getting methods: *
					********************/

					// Getting the actual element:
					virtual TVObject<T> *getActual() const {
						// Returning the object:
						if (_i<_sp->_numel) {
							// Returning the actual object:
							return _sp->_objs[_i];
						}
						else {
							// No objects:
							return NULL;
						}
					}

					/********************
					* Quering methods: *
					********************/

					// Ask for the presence of the actual element:
					virtual bool hasActual() const {
						// Checking if the actual exists:
						return _i<_sp->_numel;
					}

					// Getting the number of elements:
					virtual vector_ind_t getNumel() {
						// Returning:
						return _sp->_numel;
					}
				};

				// An iterator for all the elements:
				class TVSpVectorIterRange : public ITVIterator<TVObject<T>*> {
				protected:
					/***************
					* Attributes: *
					***************/

					// The space:
					const TVSpVector<T> *_sp;

					// The actual element:
					vector_ind_t _i;

					// The center:
					const T *_center;

					// The radius:
					const T _radius2;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					// Constructor:
					TVSpVectorIterRange(const TVSpVector<T> *sp,const T *center,T radius) 
						: _sp(sp), _center(center), _radius2(radius*radius) {
							// Resetting the index:
							reset();
					}

					/*******************
					* Moving methods: *
					*******************/

					// Resetting the iterator (restarts from the first element):
					virtual void reset() {
						// Finding the first point:
						_i = 0;
						_findNext();
					}

					// Moving forward:
					virtual void next() {
						// Moving forward:
						_i++;

						// Finding the next point:
						_findNext();
					}

					/********************
					* Getting methods: *
					********************/

					// Getting the actual element:
					virtual TVObject<T> *getActual() const {
						// Returning the object:
						if (hasActual()) {
							// Returning the actual object:
							return _sp->_objs[_i];
						}
						else {
							// No objects:
							return NULL;
						}
					}

					/********************
						* Quering methods: *
						********************/

					// Ask for the presence of the actual element:
					virtual bool hasActual() const {
						// Checking if the actual exists:
						return _i<_sp->_numel;
					}

				protected:
					/**********************
						* Protected methods: *
						**********************/

					// Moving forward:
					virtual void _findNext() {
						// Moving forward:
						while (	_i<_sp->_numel &&	// I have points:
								// This is not a good point:
								!vec_dist_L2_leq<T>(_sp->_objs[_i]->getPos(),_center,_radius2,_sp->_dim)) {
							// Next object:
							_i++;
						}
					}
				};
			};

		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPVECTOR_H_
