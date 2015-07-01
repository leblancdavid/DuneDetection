
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

#ifndef _TVRELABELTHDECAY_H_
#define _TVRELABELTHDECAY_H_

/*
 *  This class of labellers works by thresholding of the decay function
 * eventually multiplied times the saliency. This abstract class manages
 * the the features vector retrieval and computation.
 */

// All this in the TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Labelling {

			// The class:
			template <class T>
			class TVRELabelThDecay : public TVRELabel<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The threshold:
				T _th;

				// Are the saliency required?
				bool _reqSals;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVRELabelThDecay(TVObject<T> *obj,TVRELabellingGroup<T> *labelsGroup=NULL,T th=0,bool reqSals=true) : 
				TVRELabel<T>(obj,labelsGroup), _th(th), _reqSals(reqSals) {}

				// Destructor:
				~TVRELabelThDecay() {}

				/*********************
				* Abstract methods: *
				*********************/

				// The association predicate:
				virtual bool equal(TVRELabel<T> *rem) const {
					// The parameters:
					TVParameters<T> *params = TVParameters<T>::getInstance();

					// The features:
					T *features;
					const T *decs;

					// The result:
					bool res;

					// The T2 tensor associated:
					TVRETensor2<T> *T2rem = dynamic_cast<TVRETensor2<T>*>(
						rem->getObject()->getRemitter((vector_dim_t)params->getParameterInt(TVPARI_T2_INDEX)));
					if (!T2rem) return false;

					// The decay function:
					TVDFTensor2<T> *dec = dynamic_cast<TVDFTensor2<T>*>(T2rem->getDecay());
					if (!dec) return false;

					// Checking the case:
					if (_reqSals) {
						// Computing the saliency vector:
						features = T2rem->computeSals();

						// Multiplying times the decay values:
						decs = dec->getDecays();
						if (decs) {
							// Real multiplication:
							vec_multiply<T>(features,decs,getDim());
						}

						// Computing the result:
						res = threshold(features);

						// Cleaning:
						vec_free(features);
					}
					else {
						// Getting only the decays:
						decs = dec->getDecays();
						res = decs? threshold(decs): false;
					}

					// Returning:
					return res;
				}

				// The thresholding function:
				virtual bool threshold(const T *features) const=0;

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the threshold:
				T getThreshold() const {
					// Returning:
					return _th;
				}
				TVRELabelThDecay<T> *setThreshold(T th) {
					// Setting:
					_th = th;

					// Returning:
					return this;
				}

				// Getter and setter for the reqSals flag:
				bool getReqSals() const {
					// Returning:
					return _reqSals;
				}
				TVRELabelThDecay<T> *setReqSals(bool reqSals) {
					// Setting:
					_reqSals = reqSals;

					// Returning:
					return this;
				}
			};

		} // namespace Labelling
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVRELABELTHDECAY_H_
