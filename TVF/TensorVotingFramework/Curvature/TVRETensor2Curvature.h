
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

#ifndef _TVRETENSOR2CURVATURE_H_
#define _TVRETENSOR2CURVATURE_H_

/*
 *  This class allows to manage first order tensorial voting
 * to capture the directionality of votes.
 */

// All this in the TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Curvature {

			// Using the math namespace:
			using namespace TVF::Math;

			/*
			 *  This class allows to estimate and use the curvature information.
			 */
			template <class T>
			class TVRETensor2Curvature : public TVRETensor2<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The temporary buffer for the curvature computation:
				T *_dirTmp;

				// The pre-rotated directions:
				T *_dir;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVRETensor2Curvature(TVObject<T> *obj) : 
					TVRETensor2<T>(obj), _dirTmp(NULL), _dir(NULL) {}

				// Destructor:
				~TVRETensor2Curvature() {
					// Deleting the curvature vectors:
					cleanCurv();
				}

				/***************************
				 * Structure manipulation: *
				 ***************************/

				// Generating a new instance:
				TVRemitter<T> *newInstance(TVObject<T> *obj) const {
					// Using the constructor:
					TVRETensor2Curvature<T> *re = new TVRETensor2Curvature<T>(obj);

					// The dimensionality:
					vector_dim_t dim = getDim();

					// Cloning the actual initialization:
					re->_tns = vec_create_clone<T>(re->_tns,_tns,TNS_DOF_NUM(dim));
					if (_dir) {
						// Cloning:
						re->_dir = vec_create_clone<T>(re->_dir,_dir,TNS_DOF_NUM(dim-1));
					}

					// Returning:
					return re;
				}

				/*****************
				 * Voting tools: *
				 *****************/

				// Deleting the  curvature vectors:
				void cleanCurv() {
					// Deleting the vectors:
					vec_free(_dirTmp); _dirTmp = NULL;
					vec_free(_dir); _dir = NULL;
				}

				// The resetting:
				virtual void reset() {
					// T2 clean:
					TVRETensor2<T>::reset();

					// Cleaning also the curvature vectors:
					cleanCurv();
				}

				// Initialization of the voting process:
				virtual void init() {
					// The dimensionality:
					vector_dim_t dim = getDim();

					// Parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// T2 init:
					TVRETensor2<T>::init();

					// All terminated in case of no curvature computation:
					if (pars->getFlag(TVPARF_CURVATURE_COMP)) {
						// Curavture vector and count init:
						_dirTmp = vec_create_init<T>(_dirTmp,(T)0,TNS_DOF_NUM(dim));
					}
					else {
						// Removing allocated space:
						vec_free(_dirTmp); _dirTmp = NULL;
					}
				}

				/*
				 *  Voting: here MY curvature is estimated and OTHERS curvature is used.
				 *
				 * EMITTER:		rem
				 * RECEIVER:	this
				 */
				virtual void vote(TVRemitter<T> *rem) {
					// Dimensionality:
					vector_dim_t dim = getDim();

					// Parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Casting the remitter:
					TVRETensor2Curvature<T> *r = dynamic_cast<TVRETensor2Curvature<T>*>(rem);

					// The decay function:
					TVDFCurvatureBase<T> *dec = 
						dynamic_cast<TVDFCurvatureBase<T>*>(getDecay());

					// Setting them in the decay function:
					bool useCurvature = dec && r && pars->getFlag(TVPARF_CURVATURE_USE);
					if (useCurvature) {
						// Setting the curvature:
						dec->setDir(r->getDir());
					}
					else {
						// Do not use it:
						dec->setDir(NULL);
					}

					// T2 voting:
					TVRETensor2<T>::vote(rem);

					// Is the direction computation required?
					if (pars->getFlag(TVPARF_CURVATURE_COMP)) {
						// Accumulating it:
						_accumulateDir(dynamic_cast<TVRETensor2<T>*>(rem));
					}
				}

				// Updating:
				virtual void update() {
					// Dimensionality:
					vector_dim_t dim = getDim();

					// Parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// The T2 update:
					TVRETensor2<T>::update();

					// Requires the direction computation?
					if (pars->getFlag(TVPARF_CURVATURE_COMP)) {
						// Averaging:
						T *ptr = _dirTmp + dim;
						for(vector_dim_t ord=1; ord<dim; ord++) {
							// Scaling:
							vec_scale(ptr,1/(_dirTmp[ord]+(T)EPS),ord);

							// Next:
							ptr += ord;
						}

						// Substituting the old curvature estimation:
						_dir = vec_create_clone(_dir,_dirTmp+dim,TNS_DOF_NUM(dim-1));
						vec_free(_dirTmp); _dirTmp = NULL;
					}
				}

				// Computing the saliencies:
				virtual T *computeSals(T *buff=NULL) const {
					// Parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// The array to be returned:
					if (buff==NULL) 
						buff = vec_create<T>(getSaliencyNum());

					// Computing the T2 saliency values:
					TVRETensor2<T>::computeSals(buff);

					// Appending the curvature:
					if (_dir && pars->getFlag(TVPARF_CURVATURE_SALS)) {
						// Appending also the curvature (as vectors norm):
						T *dest = buff + TVRETensor2<T>::getSaliencyNum();
						for (vector_dim_t ord=1; ord<getDim(); ord++) {
							// Curvature as norm:
							dest[ord-1] = vec_norm_L2(getDir(ord),ord);
						}
					}


					// Returning:
					return buff;
				}

				// Sayng the number of saliency values generated:
				virtual vector_dim_t getSaliencyNum() const {
					// Parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// The number of saliency values:
					vector_dim_t res = TVRETensor2<T>::getSaliencyNum();

					// The previous saliency values plus the curvatures (without the ball one):
					if (_dir && pars->getFlag(TVPARF_CURVATURE_SALS)) {
						// Also the curvature:
						res += getDim() - 1;
					}

					// Returning:
					return res;
				}

				/*****************
				 * Custom tools: *
				 *****************/

				// Getter for the direcion pre-oriented:
				const T *getDir(vector_dim_t ord=1) const {
					// Check:
					if (!_dir) return NULL;

					// Checking the pointer:
					vector_dim_t dim = getDim();

					// Cannot return the zero order direction:
					ord = ord%dim;
					if (ord==0) return NULL;

					// Returning:
					return _dir + (ord*(ord-1)/2);
				}

				// Setter for the direction pre-oriented as copy:
				void setDir(T *dir,vector_dim_t ord=1) {
					// Checking the pointer:
					vector_dim_t dim = getDim();
					if (!_dir) _dir=vec_create<T>(TNS_DOF_NUM(dim-1));

					// Cannot return the zero order direction:
					ord = ord%dim;
					if (ord==0) return;

					// Computing the pointer:
					T *ptr = _dir + (ord*(ord-1)/2);

					// Copying:
					vec_create_clone(ptr,dir,ord);
				}

			protected:
				/********************
				 * Protected tools: *
				 ********************/

				// Accumulating the curvature's direction vector:
				void _accumulateDir(TVRETensor2<T> *rem) {
					// Checking:
					if (!rem) return;

					// Dimensionality:
					vector_dim_t dim = getDim();

					// Computing the rototranslated direction (getting res2):
					T *res2, *res = vec_create_clone<T>(NULL,rem->getObject()->getPos(),dim);
					vec_subtract(res,getObject()->getPos(),dim);
					res2 = mat_multiply_transpose<T>(getDecodedTensor()->eigenVectors,res,NULL,1,dim,dim,1);
					vec_free(res);

					// Getting the emitter infos:
					tensor_decoded_t<T> *tnsd = rem->getDecodedTensor();

					// Accumulating it:
					TVDFCurvatureBase<T> *decf = dynamic_cast<TVDFCurvatureBase<T>*>(getDecay());
					const T *decs = decf? decf->getDecays(): NULL,
						*curvatures = decf? decf->getCurvature(): NULL;
					T w, *ptr = _dirTmp + dim, norm;
					if (decs && curvatures) {
						// Iterating on orders:
						for (vector_dim_t ord=1; ord<dim; ord++) {
							// Computing the weight:
							w = (tnsd->eigenValues[dim-ord]-tnsd->eigenValues[dim-ord-1])*decs[ord];

							// Accumulating the vector and the weight:
							norm = vec_norm_L2(res2+dim-ord,ord);
							if (norm>(T)EPS) {
								// Scaling and accumulating:
								vec_accumulate_scaled(ptr,res2+dim-ord,curvatures[ord-1]*w/norm,ord);
								_dirTmp[ord] += w;
							}

							// Next:
							ptr += ord;
						}
					}

					// Cleaning:
					vec_free(res2);
				}
			};
		} // namespace Curvature
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVRETENSOR2CURVATURE_H_
