
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

#ifndef _TVOBJECTEXPAND_H_
#define _TVOBJECTEXPAND_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This class represents an object that can be expanded
			 * generating new expansion nodes.
			 */
			template <class T>
			class TVObjectExpand : public TVObject<T> {
			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVObjectExpand(const TVObject<T> *obj,T err=(T)1e-2) : TVObject<T>(0,true) {
					// Cloning the object:
					this->clone(obj);
				}

				/*******************************
				 * Added pure virtual methods: *
				 *******************************/

				// The order of the reconstructed structures:
				virtual vector_dim_t getStructureDim() const=0;

				/* 
				 *  Expansion: this method must expand an object in internally defined
				 * directions generating new expandible objects and inserting them in the
				 * expansion front.
				 */
				virtual void expand(const TVProcess<T> *pro,TVSpace<T> *front,ITVSaliencyMaximizer<T> *maximizer,T dt)=0;

				/***************************
				 * Tools implemented here: *
				 ***************************/

				// Checking the goodness of an object:
				bool isGood(TVObject<T> *obj) {
					// Getting the starting decoded tensor:
					if (!obj) return false;
					tensor_decoded_t<T> *tnsd = _getDecodedTensor(obj);
					if (!tnsd) return false;

					// The order:
					vector_dim_t ord = getStructureDim();

					// Computing the required saliency:
					T sal = ord==0? tnsd->eigenValues[0]:
						tnsd->eigenValues[ord]-tnsd->eigenValues[ord-1];

					// Getting the saliency threshold:
					T th = TVParameters<T>::getInstance()->getParameter(TVPAR_MARCH_THRESHOLD);

					// Returning:
					return sal>=th;
				}

				// Finding a new point from a starting object:
				T *findPoint(const TVProcess<T> *pro,T *dir,ITVSaliencyMaximizer<T> *maximizer,T dt) {
					// Getting the starting decoded tensor:
					tensor_decoded_t<T> *tnsd = _getDecodedTensor();
					if (!tnsd) return NULL;

					// Getting the initial point:
					T *p1 = _initPoint(dir,dt,_dim-getStructureDim()), *p2;

					// Maximizing the saliency:
					p2 = maximizer->maximize(pro,_pos,tnsd,_dim-getStructureDim(),p1);

					// Rototranslating the new point:
					mat_multiply<T>(tnsd->eigenVectors,p2,p1,_dim,_dim,1);
					vec_accumulate(p1,_pos,_dim);

					// Freeing unused memory:
					vec_free(p2);

					// Returning:
					return p1;
				}

				// Generating a new object near a given one (by position finding and voting):
				TVObjectExpand<T> *findObjectExpand(const TVProcess<T> *pro,T *dir,ITVSaliencyMaximizer<T> *maximizer,T dt) {
					// A new point must be computed:
					T *p = findPoint(pro,dir,maximizer,dt);
					if (!p) return NULL;

					// A new object can be created there with voting:
					TVObjectExpand<T> *obj = dynamic_cast<TVObjectExpand<T>*>(newInstance(p));
					pro->voteTarget(p,obj);

					// Correcting the aggregated position:
					if (obj->getPosAggregated()) {
						// The new point is a local copy:
						obj->setPosAggregatedDirect(false);
					}
					else {
						// The point is no longer required:
						vec_free(p);
					}

					// Returning:
					return obj;
				}

			protected:
				/********************
				 * Protected tools: *
				 ********************/

				// Getting my second order tensor decoded:
				tensor_decoded_t<T> *_getDecodedTensor(TVObject<T> *obj=NULL) {
					// Check:
					if (!obj) obj=this;

					// Getting the parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Getting the starting decoded tensor:
					TVRETensor2<T> *T2 = dynamic_cast<TVRETensor2<T>*>(obj->getRemitter(
						(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));
					if (!T2) return NULL;

					// Returning:
					return T2->getDecodedTensor();
				}

				/*
				 *  Initialization of the point finding procedure:
				 * This function allows to generate an initial estimate of the new
				 * point to be generated from a starting tensor in a given direction,
				 * considering the order of the base tensor to be used. The created
				 * point is in the reference frame of the given tensor.
				 *  dir:	Moving direction.
				 *  dt:		Discretization step.
				 *	ord:	Base tensor's order.
				 */
				T *_initPoint(T *dir,T dt,vector_dim_t ord) {
					// Getting the starting decoded tensor:
					tensor_decoded_t<T> *tnsd = _getDecodedTensor();

					// Rotating the direction vector to align it with the frame:
					T *p = mat_multiply_transpose<T>(tnsd->eigenVectors,dir,NULL,1,_dim,_dim,1);

					// Projecting the direction on the uncompressed dimensions:
					vec_create_init(p+_dim-ord,(T)0,ord);

					// Normalizing and scaling it with the given dt:
					vec_scale(p,dt/(vec_norm_L2(p,_dim-ord)+(T)EPS),_dim-ord);

					// Checking if the curvature can be used to improve this result:
					{
						// Getting the parameters:
						TVParameters<T> *pars = TVParameters<T>::getInstance();

						// Getting the curvature remitter:
						TVRETensor2Curvature<T> *T2 = dynamic_cast<TVRETensor2Curvature<T>*>(getRemitter(
							(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));

						// Checking:
						if (T2) {
							// Getting the Preoriented T1 tensor:
							const T *T1 = T2->getDir(ord);

							// Check:
							if (T1) {
								// The curvature can be used:
								T k = vec_norm_L2(T1,ord);
								T scale = 1/(k+(T)EPS);
								T delta = scale*scale - dt*dt;

								// Checking:
								if (delta>=0 && k>1e-4) {
									// Computing the scale:
									scale = scale - sqrt(delta);

									// Indexes:
									vector_ind_t pos = _dim-ord;

									// Cloning the T1 tensor:
									T *T1up = vec_create_clone<T>(NULL,T1,ord);

									// Normalizing and scaling:
									vec_scale(T1up,scale/(vec_norm_L2(T1up,ord)+(T)EPS),ord);

									// Accumulating:
									vec_accumulate(p+pos,T1up,ord);

									// Cleaning:
									vec_free(T1up);
								}
							}
						}
					}

					// Returning:
					return p;
				}
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVOBJECTEXPAND_H_
