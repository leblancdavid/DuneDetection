
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

#ifndef _TVRELABELTHDECAYSEL_H_
#define _TVRELABELTHDECAYSEL_H_

/*
 *  This type of labeller uses the maximum value of the decays for the thresholding step.
 */

// All this in the TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Labelling {

			// The class:
			template <class T>
			class TVRELabelThDecaySel : public TVRELabelThDecay<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The index of the saliency used:
				vector_dim_t _salInd;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVRELabelThDecaySel(TVObject<T> *obj,TVRELabellingGroup<T> *labelsGroup=NULL,
					T th=0,bool reqSals=true,vector_dim_t salInd=1) : 
				TVRELabelThDecay<T>(obj,labelsGroup,th,reqSals), _salInd(salInd) {}

				/***************************
				* Structure manipulation: *
				***************************/

				// Generating a new instance:
				virtual TVRemitter<T> *newInstance(TVObject<T> *obj) const {
					// Using the constructor:
					TVRELabelThDecaySel<T> *re = 
						new TVRELabelThDecaySel<T>(obj,_labelsGroup,_th,_reqSals,_salInd);

					// Returning:
					return re;
				}

				/*********************
				* Abstract methods: *
				*********************/

				// The thresholding function:
				virtual bool threshold(const T *features) const {
					// Computing the maximum value and thresholding:
					return features[_salInd]>_th;
				}

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the saliency index:
				vector_dim_t getSaliencyIndex() const {
					// Returning:
					return _salInd;
				}
				TVRELabelThDecaySel<T> *setSaliencyIndex(vector_dim_t salInd) {
					// Setting:
					_salInd = salInd;

					// Returning:
					return this;
				}
			};

		} // namespace Labelling
	} // namespace Extensions
} // namespace TVF
#endif //ndef _TVRELABELTHDECAYSEL_H_
