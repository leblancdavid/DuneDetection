
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

#ifndef _TVSPVOXHASH_H_
#define _TVSPVOXHASH_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {
		namespace Spaces {

			// Usings:
			using namespace std;
			using namespace stdext;		// Contains the hash maps!

			/*
			*  This space structure uses an hash multimap to store objects related to a blocks
			* division of the space with "radius" edge length. This allows to search only locally
			* during the range query.
			*/

			template <class T>
			class TVSpVoxHash : public TVSpVector<T> {
			protected:
				/**********
				* Types: *
				**********/

				// A map type:
				typedef hash_multimap<vector_ind_t,TVObject<T>*> _hash_t;

				// A key/value pair:
				typedef pair<vector_ind_t,TVObject<T>*> _hash_pair;

				// An hash iterator:
				typedef typename _hash_t::iterator _hash_iterator;

				// An hash range:
				typedef pair<_hash_iterator,_hash_iterator> _hash_range;

				/***************
				* Attributes: *
				***************/

				// The image of objects:
				_hash_t *_hash;

				// The sizes of the image (aggregated):
				T *_imgSizes;

				// The starting position in the image (aggregated):
				T *_imgFrom;

				// The radius:
				T _radius;

				// The number of voxels:
				vector_ind_t _numVoxels;

				// The layer sizes:
				vector_ind_t *_layerSize;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// The default constructor:
				TVSpVoxHash(T *imgSizes=NULL,T *imgFrom=NULL,T radius=0,vector_dim_t dim=0) 
						: TVSpVector<T>(dim), _hash(NULL), _imgSizes(imgSizes), _imgFrom(imgFrom),
							_radius(radius), _layerSize(NULL) {
					// Setting the image sizes and from:
					setImageSizes(_imgSizes);
					setImageFrom(_imgFrom);

					// Creating the hash map:
					_hash = new _hash_t;
				}

				// Destructor:
				~TVSpVoxHash() {
					// Deleting the image:
					delete _hash;
					delete _layerSize;
				}

				// Generating a new (empty) instance:
				virtual TVSpace<T> *newInstance(vector_dim_t dim=0) const {
					// Creating a new vector:
					if (dim==0) dim = _dim;
					return new TVSpVoxHash<T>(_imgSizes,_imgFrom,_radius,dim);
				}

				/***************************
				* Structure manipulation: *
				***************************/

				// Adding an object:
				virtual void add(TVObject<T> *obj) {
					// Adding the object using the superclass:
					TVSpVector<T>::add(obj);

					// Computing the index:
					vector_ind_t finalInd = _point2index(obj->getPos());

					// Adding the new object in the hash:
					_hash->insert(_hash_pair(finalInd,obj));
				}

				// Extracting an object:
				virtual TVObject<T> *extract(TVObject<T> *obj) {
					// Computing the index:
					vector_ind_t finalInd = _point2index(obj->getPos());

					// Removing the object pointer from the hash:
					for (_hash_iterator i=_hash->find(finalInd); i!=_hash->end(); i++) {
						// Checking:
						if (i->second==obj) {
							// Removing only the pointer (the object is deleted from the superclass):
							_hash->erase(i);

							// Done:
							break;
						}
					}

					// The superclass remove:
					return TVSpVector<T>::extract(obj);
				}

				// Extracting all the objects:
				virtual void empty() {
					// Cleaning the hash:
					_hash->clear();

					// The clean for the super class:
					TVSpVector<T>::empty();
				}

				// Cleaning (removing the objects):
				virtual void clean() {
					// Cleaning the hash:
					_hash->clear();

					// The clean for the super class:
					TVSpVector<T>::clean();
				}

				// Getting a range of objects:
				virtual typename TVObject<T>::iterator_t *range(const T *center, T radius=-1) const {
					// Managing the radius:
					if (radius<0) {
						// Getting the default one:
						radius = TVParameters<T>::getInstance()->getParameter(TVPAR_RADIUS);
					}

					// Creating an iterator with me:
					return new TVSpVoxHashIterRange(this,center,radius);
				}

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the sizes:
				vector_dim_t *getImageSizes() const {
					// Returning:
					return _imgSizes;
				}
				TVSpVoxHash *setImageSizes(T *imgSizes) {
					// Setting:
					_imgSizes = imgSizes;

					// Checking for a simple deallocation:
					if (imgSizes==NULL) return this;

					// Allocating the layer sizes:
					if (_layerSize) delete _layerSize;
					_layerSize = new vector_ind_t[_dim];

					// Computing the number of voxels and the layer size:
					vector_dim_t i;
					for (i=0,_numVoxels=1; i<_dim; i++) {
						// The _numVoxels:
						_numVoxels *= (vector_ind_t)ceil(_imgSizes[i]/_radius);

						// The _layerSize:
						_layerSize[i] = _numVoxels;
					}

					// Returning:
					return this;
				}

				// Getter and setter for the from:
				vector_dof_t *getImageFrom() const {
					// Returning:
					return _imgFrom;
				}
				// WARNING: changing the from on a space with objects can cause erroneous range queries!
				TVSpVoxHash *setImageFrom(T *imgFrom) {
					// Saving it:
					_imgFrom = imgFrom;

					// Returning:
					return this;
				}

				// Getter and setter for the radius:
				T getRadius() const {
					// Returning:
					return _radius;
				}
				// WARNING: it is a bad idea to set a value for the radius different from the real
				//			one used at creation time, only empty spaces should be modified with
				//			this method.
				TVSpVoxHash *setRadius(T radius) {
					// Setting:
					_radius = radius;

					// Returning:
					return this;
				}

			protected:
				/*************************
				* My protected methods: *
				*************************/

				// Given a point computes the index (_numVoxels==error):
				vector_ind_t _point2index(const T *pos) const {
					// - Computing the index:
					vector_ind_t finalInd=0,i;
					T ind;
					for (i=0; i<_dim; i++) {
						// Computing an index:
						ind = pos[i] - _imgFrom[i];

						// Checking:
						if (ind<0 || ind>=_imgSizes[i]) {
							// Out of the image:
							return _numVoxels;
						}

						// Saving:
						if (i>0) {
							// General case:
							finalInd = finalInd + _layerSize[i-1]*(vector_ind_t)(ind/_radius);
						}
						else {
							// Special case:
							finalInd = finalInd + (vector_dim_t)(ind/_radius);
						}
					}

					// Returning:
					return finalInd;
				}

				// Given an index array computes the index value (_numVoxels==error):
				vector_ind_t _sub2index(const vector_dim_t *inds) const {
					// Computing the index:
					vector_ind_t finalInd=0,i;
					for (i=0; i<_dim; i++) {
						// Checking:
						if (inds[i]<0 || inds[i]>(vector_dim_t)(_imgSizes[i]/_radius)) {
							// Out of the image:
							return _numVoxels;
						}

						// Saving:
						if (i>0) {
							// General case:
							finalInd = finalInd + _layerSize[i-1]*inds[i];
						}
						else {
							// Special case:
							finalInd = finalInd + inds[i];
						}
					}

					// Returning:
					return finalInd;
				}

			protected:

				/********************************************************************
				*                            My iterators:                         *
				********************************************************************/

				// A range iterator:
				class TVSpVoxHashIterRange : public ITVIterator<TVObject<T>*> {
				protected:
					/***************
						* Attributes: *
						***************/

					// The space:
					const TVSpVoxHash<T> *_sp;

					// The actual element's indexes:
					vector_dim_t *_inds;

					// The actual elements' iterator:
					_hash_range _i;

					// The center:
					const T *_center;

					// The radius:
					const T _radius, _radius2;

					// Has actual:
					bool _hasActual;

				public:
					/********************************
					* Constructors and destructor: *
					********************************/

					// Constructor:
					TVSpVoxHashIterRange(const TVSpVoxHash<T> *sp,const T *center,T radius) 
						: _sp(sp), _center(center), _radius(radius), _radius2(radius*radius) {
							// Allocating the index:
							_inds = new vector_dim_t[_sp->getDim()];

							// Resetting the index:
							reset();
					}

					// Destructor:
					~TVSpVoxHashIterRange() {
						// Deallocating the index:
						delete _inds;
					}

					/*******************
					* Moving methods: *
					*******************/

					// Resetting the iterator (restarts from the first element):
					virtual void reset() {
						// Setting the index to the upper leftmost corner:
						for(int i=0; i<_sp->getDim(); i++) {
							// Setting the values:
							_inds[i] = (vector_dim_t)MAX(((_center[i] - _sp->_imgFrom[i])/_radius) - 1,0);
						}

						// The first iterator must be created:
						_i = _sp->_hash->equal_range(_sp->_sub2index(_inds));

						// Finding the first point:
						_hasActual = true;
						_findNext();
					}

					// Moving forward:
					virtual void next() {
						// Moving forward:
						if (_i.first!=_i.second) _i.first++;

						// Finding the next point from here:
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
							return _i.first->second;
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
						return _hasActual;
					}

				protected:
					/**********************
					* Protected methods: *
					**********************/

					// Moving forward:
					virtual void _findNext() {
						// Checking:
						if (!_hasActual) return;

						// The index:
						vector_dim_t i,dim = _sp->getDim();

						// Moving forward:
						while (1) {
							// Is this good?
							while (	_i.first!=_i.second && 
									!vec_dist_L2_leq<T>(_i.first->second->getPos(),_center,_radius2,dim)) {
								// Next:
								_i.first++;
							}

							// Done?
							if (_i.first!=_i.second) {
								// Done:
								return;
							}

							// No good, checking the next voxel:
							_inds[0]++;

							// Adjusting and checking the index:
							i = 0;
							while (i<dim-1 && (	_inds[i]>=(vector_dim_t)ceil(_sp->_imgSizes[i]/_radius) || 
												_inds[i]>(vector_dim_t)((_center[i] - _sp->_imgFrom[i])/_radius)+1)) {
								// Correcting the index:
								_inds[i] = (vector_dim_t)MAX(((_center[i] - _sp->_imgFrom[i])/_radius) - 1,0);

								// Next:
								_inds[++i]++;
							}

							// Is this inside?
							if (_inds[dim-1]>(vector_dim_t)((_center[dim-1] - _sp->_imgFrom[dim-1])/_radius)+1) {
								// No, no other voxels:
								_hasActual = false;
								return;
							}

							// Getting the actual voxel:
							_i = _sp->_hash->equal_range(_sp->_sub2index(_inds));
						}
					}
				};
			};

		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPVOXHASH_H_
