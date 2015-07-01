
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

#ifndef _TVOBJECTEXPANDCURVE_H_
#define _TVOBJECTEXPANDCURVE_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This class represents an expansion object for curves.
			 */

			template <class T>
			class TVObjectExpandCurve : public TVObjectExpand<T> {
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
				TVObjectExpandCurve(const TVObject<T> *obj) : 
						TVObjectExpand<T>(obj), _dir(0) {
					// Getting the parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Getting the first order tensor:
					TVRETensor1<T> *T1 = dynamic_cast<TVRETensor1<T>*>(getRemitter(
						(vector_dim_t)pars->getParameterInt(TVPARI_T1_INDEX)));
					TVRETensor2<T> *T2 = dynamic_cast<TVRETensor2<T>*>(getRemitter(
						(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));
					if (T1 && T2) {
						// Reorienting the T1 tensor:
						T v = vec_dot(T2->getDecodedTensor()->eigenVectors,T1->getTensor(),_dim);

						// Projecting and thresholding:
						if (ABS(v)>=pars->getParameter(TVPAR_EXTREMA_THRESHOLD)) {
							// This can be an extrema:
							_dir = v>0? 1: -1;
						}
					}
				}

				// A cloning method:
				virtual TVObjectExpandCurve<T> *newInstance(T *pos) const {
					// Creating the new object:
					TVObjectExpandCurve<T> *obj = new TVObjectExpandCurve<T>(this);

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
					return 1;
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
					TVObjectExpandCurve<T> *obj;
					T *dir = NULL;

					// Expansion in a direction:
					if (_dir>=0) {
						// Expanding along the positive direction:
						obj = dynamic_cast<TVObjectExpandCurve<T>*>(
							findObjectExpand(pro,tnsd->eigenVectors,maximizer,dt));

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
							if (obj) delete obj;
						}
					}

					// Expanding in the other direction:
					if (_dir<=0) {
						// Inverting the first eigenvector:
						dir = vec_create_clone(dir,tnsd->eigenVectors,_dim);
						vec_scale(dir,(T)-1,_dim);

						// Expanding along the negative direction:
						obj = dynamic_cast<TVObjectExpandCurve<T>*>(
							findObjectExpand(pro,dir,maximizer,dt));

						// Checking the saliency:
						if(isGood(obj)) {
							// Computing the direction:
							dir = vec_create_clone(dir,obj->getPos(),_dim);
							vec_subtract(dir,_pos,_dim);

							// Setting it:
							obj->setDirection(dir);

							// Adding the object to the front:
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

					// Cleaning:
					vec_free(dir);
				}

				/**************
				 * Accessors: *
				 **************/

				// Setters for the direction:
				TVObjectExpandCurve<T> *setDirection(T *dir) {
					// Getting the starting decoded tensor:
					tensor_decoded_t<T> *tnsd = _getDecodedTensor();
					if (!tnsd) return NULL;

					// The same direction is 1, the opposite one is -1:
					if (vec_dot(tnsd->eigenVectors,dir,_dim)>=0) {
						// Same direction:
						_dir = 1;
					}
					else {
						// Opposite direction:
						_dir = -1;
					}

					// Returning:
					return this;
				}
				TVObjectExpandCurve<T> *setDirectionDirect(short dir) {
					// Setting:
					_dir = dir;

					// Returning:
					return this;
				}
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVOBJECTEXPANDCURVE_H_
