
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

#ifndef _TVOBJECTEXPANDSURFACE_H_
#define _TVOBJECTEXPANDSURFACE_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This class represents an expansion object for surfaces.
			 */

			template <class T>
			class TVObjectExpandSurface : public TVObjectExpand<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The expansion direction:
				short _dir;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVObjectExpandSurface(const TVObject<T> *obj) : 
						TVObjectExpand<T>(obj), _dir(15) {
					// Getting the parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Getting the first order tensor:
					TVRETensor1<T> *T1 = dynamic_cast<TVRETensor1<T>*>(getRemitter(
						(vector_dim_t)pars->getParameterInt(TVPARI_T1_INDEX)));
					TVRETensor2<T> *T2 = dynamic_cast<TVRETensor2<T>*>(getRemitter(
						(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));
					if (T1 && T2) {
						// Reorienting the T1 tensor:
						T v, v1, v2,
							th = pars->getParameter(TVPAR_EXTREMA_THRESHOLD);
						v1 = vec_dot(T2->getDecodedTensor()->eigenVectors,T1->getTensor(),_dim);
						v2 = vec_dot(T2->getDecodedTensor()->eigenVectors+_dim,T1->getTensor(),_dim);
						v = v1*v1 + v2*v2;

						// Projecting and thresholding:
						if (v >= th*th) {
							// This can be an extrema, computing the expansion directions:
							_dir =	(v1>0?1:2) | // First eigenvector:
									(v2>0?4:8);	// Second eigenvector:
						}
					}
				}

				// A cloning method:
				virtual TVObjectExpandSurface<T> *newInstance(T *pos) const {
					// Creating the new object:
					TVObjectExpandSurface<T> *obj = new TVObjectExpandSurface<T>(this);

					// Setting the poistion:
					obj->setPos(pos);

					// Setting the expansion direction:
					obj->setDirectionDirect(_dir);

					// Returning:
					return obj;
				}

				/***********************************
				 * Virtual methods implementation: *
				 ***********************************/

				// The order of the reconstructed structures:
				virtual vector_dim_t getStructureDim() const {
					// Returning:
					return 2;
				}

				/*
				 * Expansion:
				 *  IN:
				 *   pro:	The process that contains voters.
				 *   front:	The destination space.
				 *   dt:	The discretization step.
				 */
				virtual void expand(const TVProcess<T> *pro,TVSpace<T> *front,ITVSaliencyMaximizer<T> *maximizer,T dt) {
					// Getting the starting decoded tensor:
					tensor_decoded_t<T> *tnsd = _getDecodedTensor();
					if (!tnsd) return;

					// Expansion object:
					TVObjectExpandSurface<T> *obj;
					T *dir = NULL;

					// Checking every direction:
					int actualBitShift = 0;
					for (int eig=0; eig<2; eig++) {
						for (int side=1; side>-2; side-=2) {
							// Checking the actual bit:
							if (_dir&(0x01<<actualBitShift)) {
								// Checking the direction:
								if (side>0) {
									// Working on the actual eigenvector/side:
									obj = dynamic_cast<TVObjectExpandSurface<T>*>(
										findObjectExpand(pro,tnsd->eigenVectors+_dim*eig,maximizer,dt));
								}
								else {
									// Opposite direction:
									dir = vec_create_clone(dir,tnsd->eigenVectors+_dim*eig,_dim);
									vec_scale(dir,(T)-1,_dim);

									// Working on the actual eigenvector/side:
									obj = dynamic_cast<TVObjectExpandSurface<T>*>(
										findObjectExpand(pro,dir,maximizer,dt));
								}

								// Checking the saliency:
								if(isGood(obj)) {
									// Computing the direction:
									dir = vec_create_clone(dir,obj->getPos(),_dim);
									vec_subtract(dir,_pos,_dim);

									// Setting it:
									obj->setDirection(dir);

									// Adding the objct to the front:
									front->add(obj);
								}
								else {
									// Useless:
									if (obj) {
										delete obj;
										obj = NULL;
									}
								}
							}

							// Next bit:
							actualBitShift++;
						}
					}

					// Cleaning:
					vec_free(dir);
				}

				/**************
				 * Accessors: *
				 **************/

				// Setters for the direction:
				TVObjectExpandSurface<T> *setDirection(T *dir) {
					// Getting the starting decoded tensor:
					tensor_decoded_t<T> *tnsd = _getDecodedTensor();
					if (!tnsd) return NULL;

					// Setting multiple directions when not well decided:
					T val;
					// First eigenvector:
					val = vec_dot(tnsd->eigenVectors,dir,_dim);
					if (abs(val)<1e-2) _dir = 3;
					else _dir = val>=0? 1: 2;
					// Second eigenvector:
					val = vec_dot(tnsd->eigenVectors+_dim,dir,_dim);
					if (abs(val)<1e-2) _dir |= 12;
					else _dir |= val>=0? 4: 8;

					// Returning:
					return this;
				}
				TVObjectExpandSurface<T> *setDirectionDirect(short dir) {
					// Setting:
					_dir = dir;

					// Returning:
					return this;
				}
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVOBJECTEXPANDSURFACE_H_
