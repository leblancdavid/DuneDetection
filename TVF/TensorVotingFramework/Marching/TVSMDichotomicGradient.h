
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

#ifndef _TVSMDICHOTOMICGRADIENT_H_
#define _TVSMDICHOTOMICGRADIENT_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This class implements the saliency maximization algorithm based on the gradient
			 * dochotomic search.
			 */

			template <class T>
			class TVSMDichotomicGradient : public ITVSaliencyMaximizer<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The discretization step for the gradient computation:
				T _dt;

				// Number of iterations:
				unsigned short _iters;

				// The order of the submanifold:
				vector_dim_t _ord;

				// The actual decoded tensor:
				const tensor_decoded_t<T> *_tnsd;

				// The actual position:
				const T *_pos;

				// The process:
				const TVProcess<T> *_pro;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVSMDichotomicGradient(vector_dim_t ord,T dt,unsigned short iters=1) 
					: _ord(ord), _dt(dt), _iters(iters) {}

				/***************************************
				 * Implementation of abstract methods: *
				 ***************************************/

				/*
				 *  This method allows to maximize the saliency starting from a point p0 and moving on the
				 * the ord compressed dimensions of the given tensor tnsd. The given and returned points are
				 * defined in the reference frame of the given second order tensor.
				 * IN:
				 *  pro:	The process used for the voting on arbitrary target points.
				 *  pos:	The position of the reference frame.
				 *  tnsd:	The tensor defining the orientation of the constraining subspace.
				 *  ord:	The order of the constrining subspace (number of compressed dimensions).
				 *  p0:		The starting point.
				 *  dest:	The destination vector (allocated if NULL).
				 * OUT:
				 *  The found maxima point.
				 */
				virtual T *maximize(const TVProcess<T> *pro,const T *pos,const tensor_decoded_t<T> *tnsd,
						vector_dim_t ord,const T *p1,T *dest=NULL) {
					// The dimensionality:
					vector_dim_t dim = pro->getSpace()->getDim();

					// Local vectors:
					T *p2, *p3, *g1, *g2, *g3, *myDir;

					// Setting local infos:
					_pos = pos;
					_tnsd = tnsd;
					_pro = pro;

					// Allocating:
					g1 = vec_create<T>(dim);
					g2 = vec_create<T>(dim);
					g3 = vec_create<T>(dim);
					p2 = vec_create<T>(dim);
					p3 = vec_create<T>(dim);
					myDir = vec_create<T>(dim);

					// Creating a copy of p1:
					dest = vec_create_clone(dest,p1,dim);

					// Dicotomic gradient ascend:
					for(int i=0; i<_iters; i++) {	// Doing it _iters times:
						// Finding the second opposite point:
						_findOpposite(dest,p2,g1,g2);

						// Dichotomic search:
						_findMaxDichotomic(dest,p2,p3,g1,g2,g3,myDir);
					}

					// Freeing unused memory:
					vec_free(g1);
					vec_free(g2);
					vec_free(g3);
					vec_free(p2);
					vec_free(p3);
					vec_free(myDir);

					// Returning:
					return dest;
				}

			protected:
				/********************
				 * Protected tools: *
				 ********************/

				// Computing the gradient vector on the uncompressed dimensions:
				T *_computeGradient(T *p,T *dest) const {
					// Getting the parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Getting the dimensionality of the manifold:
					vector_dim_t dim = _pro->getSpace()->getDim();
					vector_dim_t manOrd = dim-_ord;
					if (manOrd<1 || manOrd>=dim) return NULL;

					// Obtaining the real space point:
					T *rp = vec_create<T>(dim);
					dest = mat_multiply(_tnsd->eigenVectors,p,dest,dim,dim,1);
					vec_accumulate(dest,_pos,dim);

					// Voting there:
					TVObject<T> *obj = _pro->voteTarget(dest);

					// Computing the saliency there:
					bool useCentral = pars->getFlag(TVPARF_GRADIENT_APPROX_CENTRAL);
					T sal;
					TVRETensor2<T> *T2;
					tensor_decoded_t<T> *tnsd2;
					if (!useCentral) {
						// Getting the starting decoded tensor:
						T2 = dynamic_cast<TVRETensor2<T>*>(obj->getRemitter(
							(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));

						// Getting the starting decoded tensor:
						tnsd2 = T2? T2->getDecodedTensor(): NULL;
						if (!tnsd2) {
							// BAD!
							delete obj;
							return NULL;
						}

						// Computing the local saliency value:
						sal = tnsd2->eigenValues[manOrd]-tnsd2->eigenValues[manOrd-1];
					}

					// Iterating on all the dimensions:
					for (vector_dim_t i=0; i<dim; i++) {
						// If the central gradient approximation must be used:
						if (useCentral) {
							// Moving the point in this direction:
							dest[i] -= _dt;

							// Voting there:
							_pro->voteTarget(dest,obj);

							// The decoded tensor:
							T2 = dynamic_cast<TVRETensor2<T>*>(obj->getRemitter(
								(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));
							tnsd2 = T2->getDecodedTensor();
							sal = tnsd2->eigenValues[manOrd]-tnsd2->eigenValues[manOrd-1];

							// Correcting the point:
							dest[i] += _dt;
						}

						// Moving the point in this direction:
						dest[i] += _dt;

						// Voting there:
						_pro->voteTarget(dest,obj);

						// The decoded tensor:
						T2 = dynamic_cast<TVRETensor2<T>*>(obj->getRemitter(
							(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));
						tnsd2 = T2->getDecodedTensor();

						// Correcting the point:
						dest[i] -= _dt;

						// Updating the gradient:
						rp[i] = ((tnsd2->eigenValues[manOrd]-tnsd2->eigenValues[manOrd-1])-sal)/(useCentral? 2*_dt: _dt);
					}

					// Reorienting the gradient:
					mat_multiply_transpose(_tnsd->eigenVectors,rp,dest,1,dim,dim,1);

					// Projecting it on the compressed dimensions:
					vec_create_init(dest,(T)0,manOrd);

					// Cleaning:
					delete obj;
					vec_free(rp);

					// Returning:
					return dest;
				}

				// Finding the opposite point:
				void _findOpposite(T *start,T *dest,T *gs,T *gd) const {
					// The dimensionality:
					vector_dim_t dim = _pro->getSpace()->getDim();

					// The gradient:
					_computeGradient(start,gs);
					vec_scale(gs,_dt/(vec_norm_L2(gs,dim)+(T)EPS),dim);

					// Finding the opposite point:
					vec_create_clone(dest,start,dim);
					do {
						// The next point:
						vec_accumulate(dest,gs,dim);
						vec_scale(gs,(T)2,dim);

						// A new gradient:
						gd = _computeGradient(dest,gd);
					} while(vec_dot(gs,gd,dim)>0);
				}

				// Dichotomic max search:
				void _findMaxDichotomic(T *p1,T *p2,T *p3,T *g1,T *g2,T *g3,T *dir) const {
					// The dimensionality:
					vector_dim_t dim = _pro->getSpace()->getDim();

					// Error:
					T delta, *tmp, err = _dt;

					// The direction:
					vec_create_clone(dir,p2,dim);
					vec_subtract(dir,p1,dim);

					// Dichotomic search (res in p1):
					while((delta=vec_dist_L2(p1,p2,dim))>err) {
						// Getting the central point:
						delta /= 2;
						vec_create_clone(p3,p1,dim);
						vec_accumulate(p3,p2,dim);
						vec_scale(p3,(T)0.5,dim);

						// The middle gradient:
						_computeGradient(p3,g3);

						// Selecting (checking if p1-p3 IS BAD):
						if (vec_dot(dir,g3,dim)*vec_dot(g1,dir,dim)>0) {
							// The middle becomes the first:
							tmp = p3; p3 = p1; p1 = tmp;
							tmp = g3; g3 = g1; g1 = tmp;
						}
						else {
							// The middle becomes the second:
							tmp = p3; p3 = p2; p2 = tmp;
							tmp = g3; g3 = g2; g2 = tmp;
						}
					}

					// The used point is the mean:
					vec_accumulate(p1,p2,dim);
					vec_scale(p1,(T)0.5,dim);
				}
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVSMDICHOTOMICGRADIENT_H_
