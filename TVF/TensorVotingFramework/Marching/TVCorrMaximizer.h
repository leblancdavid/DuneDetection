
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

#ifndef _TVCORRMAXIMIZER_H_
#define _TVCORRMAXIMIZER_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This type of corrector uses a maximizer to maximize the saliency on
			 * the linear subspace identified by the point's tensor and the structure order.
			 */

			template <class T>
			class TVCorrMaximizer : public ITVCorrector<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The maximizer:
				ITVSaliencyMaximizer<T> *_maximizer;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVCorrMaximizer(ITVSaliencyMaximizer<T> *maximizer) : _maximizer(maximizer) {}

				/***************************************
				 * Implementation of abstract methods: *
				 ***************************************/

				/*
				 *  Correction of the points positions in a space:
				 * IN:
				 *  pro:	The process with the space to be modified.
				 *      
				 */
				virtual void correct(TVProcess<T> *pro,vector_dim_t ord) {
					// Getting the parameters:
					TVParameters<T> *pars = TVParameters<T>::getInstance();

					// Indexes:
					TVSpace<T> *sp = pro->getSpace();
					vector_ind_t i, numel = sp->getSize();

					// Collecting objects:
					TVObject<T> **objs = new TVObject<T>*[numel];
					TVObject<T>::iterator_t *iter;
					for (i=0,iter=sp->getIterator(); iter->hasActual(); iter->next(),i++) {
						// Saving the pointer:
						objs[i] = iter->getActual();
					}
					delete iter;

					// Local informations:
					TVRETensor2<T> *T2;
					tensor_decoded_t<T> *tnsd;
					vector_dim_t dim = objs[0]->getDim(), strDim = dim-ord;
					T *p1 = NULL, *p2 = NULL, *pos;

					// Iterating on objects:
					for (i=0; i<numel; i++) {
						// Extracting an object:
						sp->extract(objs[i]);

						// Correcting the point:

						// - Getting the starting decoded tensor:
						T2 = dynamic_cast<TVRETensor2<T>*>(objs[i]->getRemitter(
							(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));
						tnsd = T2->getDecodedTensor();

						// - Computing the initial point:
						pos = objs[i]->getPos();
						p1 = vec_create_init(p1,(T)0,dim);

						// - Maximizing the saliency:
						p2 = _maximizer->maximize(pro,pos,tnsd,dim-strDim,p1,p2);

						// - Rototranslating the new point:
						mat_multiply<T>(tnsd->eigenVectors,p2,p1,dim,dim,1);
						vec_accumulate(p1,pos,dim);

						// - Setting the new point position:
						objs[i]->setPosAggregated(false);
						objs[i]->setPos(p1);

						// Inserting the corrected object:
						sp->add(objs[i]);
					}

					// Final correction on votes:
					pro->votingPass();

					// Freeing unused memory:
					vec_free(p1);
					vec_free(p2);
					delete objs;
				}
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVCORRMAXIMIZER_H_
