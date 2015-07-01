
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

#ifndef _TVSPIMG_H_
#define _TVSPIMG_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace DataStructures {
		namespace Spaces {

			/*
			*  This class represents a space data structure that allows to work with
			* images in a memory-consuming but fast way. All the pixels/voxels of the
			* image are used to store a pointer to an object, every pixel/voxel can
			* contain only a single object and only the last added object is mantained.
			* WARNING: The full iterator do not mantain the order of objects in the image!
			*/

			template <class T>
			class TVSpImg : public TVSpVector<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The image of objects:
				TVObject<T> **_image;

				// The sizes of the image (aggregated):
				vector_dim_t *_imgSizes;

				// The starting position in the image (aggregated):
				vector_dof_t *_imgFrom;

				// The number of voxels:
				vector_ind_t _numVoxels;

				// The layer sizes:
				vector_ind_t *_layerSize;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// The default constructor:
				TVSpImg(vector_dim_t *imgSizes=NULL,vector_dof_t *imgFrom=NULL,vector_dim_t dim=0) 
						: TVSpVector<T>(dim), _image(NULL), _imgSizes(imgSizes), _imgFrom(imgFrom), _layerSize(NULL) {
					// Setting the image sizes and from:
					setImageSizes(_imgSizes);
					setImageFrom(_imgFrom);
				}

				// Destructor:
				~TVSpImg() {
					// Deleting the image:
					delete _image;
					delete _layerSize;
				}

				// Generating a new (empty) instance:
				virtual TVSpace<T> *newInstance(vector_dim_t dim=0) const {
					// Creating a new vector:
					if (dim==0) dim = _dim;
					return new TVSpImg<T>(_imgSizes,_imgFrom,dim);
				}

				/***************************
				* Structure manipulation: *
				***************************/

				// Adding an object:
				virtual void add(TVObject<T> *obj) {
					// Adding the object using the superclass:
					TVSpVector<T>::add(obj);

					// Adding it to the image:
					const T *pos = obj->getPos();

					// - Computing the index:
					vector_ind_t finalInd = _point2index(pos);

					// Checking:
					if (finalInd<_numVoxels) {
						// Setting it:
						_image[finalInd] = obj;
					}
				}

				// Extracting an object:
				virtual TVObject<T> *extract(TVObject<T> *obj) {
					// Removing the object pointer from the image:
					for (vector_ind_t i=0; i<_numVoxels; i++) {
						// Checking:
						if (_image[i]==obj) {
							// Removing only the pointer (the object is deleted from the superclass):
							_image[i]=NULL;

							// Done:
							break;
						}
					}

					// The superclass extract:
					return TVSpVector<T>::extract(obj);
				}

				// Extracting all the objects:
				virtual void empty() {
					// Cleaning the pointers:
					memset(_image,0,_numVoxels*sizeof(TVObject<T>*));

					// The clean for the super class:
					TVSpVector<T>::empty();
				}

				// Cleaning (removing the objects):
				virtual void clean() {
					// Cleaning the pointers:
					memset(_image,0,_numVoxels*sizeof(TVObject<T>*));

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
					return new TVSpImgIterRange(this,center,radius);
				}

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the sizes:
				vector_dim_t *getImageSizes() const {
					// Returning:
					return _imgSizes;
				}
				TVSpImg *setImageSizes(vector_dim_t *imgSizes) {
					// Setting:
					_imgSizes = imgSizes;

					// Checking for a simple deallocation:
					if (_image) {
						// Deleting:
						delete _image;
						_image = NULL;
					}
					if (imgSizes==NULL) return this;

					// Allocating the layer sizes:
					if (_layerSize) delete _layerSize;
					_layerSize = new vector_ind_t[_dim];

					// Computing the number of voxels and the layer size:
					vector_dim_t i;
					for (i=0,_numVoxels=1; i<_dim; i++) {
						// The _numVoxels:
						_numVoxels *= _imgSizes[i];

						// The _layerSize:
						_layerSize[i] = _numVoxels;
					}

					// Allocating the image:
					_image = new TVObject<T>*[_numVoxels];

					// Setting all to NULL:
					memset(_image,0,_numVoxels*sizeof(TVObject<T>*));

					// Returning:
					return this;
				}

				// Getter and setter for the from:
				vector_dof_t *getImageFrom() const {
					// Returning:
					return _imgFrom;
				}
				// WARNING: changing the from on a space with objects can cause erroneous range queries!
				TVSpImg *setImageFrom(vector_dof_t *imgFrom) {
					// Saving it:
					_imgFrom = imgFrom;

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
					int ind;
					for (i=0; i<_dim; i++) {
						// Computing an index:
						ind = (int)(pos[i]-_imgFrom[i]+(T)0.5);

						// Checking:
						if (ind<0 || ind>=_imgSizes[i]) {
							// Out of the image:
							return _numVoxels;
						}

						// Saving:
						if (i>0) {
							// General case:
							finalInd = finalInd + _layerSize[i-1]*ind;
						}
						else {
							// Special case:
							finalInd = finalInd + (vector_dim_t)ind;
						}
					}

					// Returning:
					return finalInd;
				}

				// Given an index array computes the index value (_numVoxels==error):
				vector_ind_t _sub2index(const vector_dim_t *inds) const {
					// - Computing the index:
					vector_ind_t finalInd=0,i;
					for (i=0; i<_dim; i++) {
						// Checking:
						if (inds[i]<0 || inds[i]>=_imgSizes[i]) {
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
				class TVSpImgIterRange : public ITVIterator<TVObject<T>*> {
				protected:
					/***************
					* Attributes: *
					***************/

					// The space:
					const TVSpImg<T> *_sp;

					// The actual element's indexes:
					vector_dim_t *_inds;

					// The actual element's pointer:
					vector_ind_t _i;

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
					TVSpImgIterRange(const TVSpImg<T> *sp,const T *center,T radius) 
						: _sp(sp), _center(center), _radius(radius), _radius2(radius*radius) {
							// Allocating the index:
							_inds = new vector_dim_t[_sp->getDim()];

							// Resetting the index:
							reset();
					}

					// Destructor:
					~TVSpImgIterRange() {
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
							_inds[i] = (vector_dim_t)MAX((_center[i] - _sp->_imgFrom[i] - _radius + (T)0.5),0);
						}

						// Finding the first point:
						_hasActual = true;
						_findNext();
					}

					// Moving forward:
					virtual void next() {
						// Moving forward:
						_inds[0]++;

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
							return _sp->_image[_i];
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
							// Checking the index:
							i = 0;
							while (i<dim-1 && (_inds[i]>=_sp->_imgSizes[i] || _inds[i]>_center[i] - _sp->_imgFrom[i] + _radius)) {
								// Correcting the index:
								_inds[i] = (vector_dim_t)MAX((_center[i] - _sp->_imgFrom[i] - _radius + (T)0.5),0);

								// Next:
								_inds[++i]++;
							}

							// Correcting _i:
							_i = _sp->_sub2index(_inds);

							// Is this inside?
							if (_i>=_sp->_numVoxels || _inds[dim-1]>_center[dim-1] - _sp->_imgFrom[dim-1] + _radius) {
								// No other voxels:
								_hasActual = false;
								return;
							}

							// Is this good?
							if (_sp->_image[_i]!=NULL &&
								vec_dist_L2_leq<T>(_sp->_image[_i]->getPos(),_center,_radius2,dim)) {
								// Done:
								return;
							}

							// Next:
							_inds[0]++;
							_i++;
						}
					}
				};
			};

		} // namespace Spaces
	} // namespace DataStructures
} // namespace TVF

#endif //ndef _TVSPIMG_H_
