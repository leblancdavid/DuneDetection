
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

#ifndef _TVRELABELTHDECAYMAX_H_
#define _TVRELABELTHDECAYMAX_H_

/*
 *  This type of labeller uses the maximum value of the decays for the thresholding step.
 */

// All this in the TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Labelling {

			// The class:
			template <class T>
			class TVRELabelThDecayMax : public TVRELabelThDecay<T> {
			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVRELabelThDecayMax(TVObject<T> *obj,TVRELabellingGroup<T> *labelsGroup=NULL,
					T th=0,bool reqSals=true) : 
				TVRELabelThDecay<T>(obj,labelsGroup,th,reqSals) {}

				/***************************
				* Structure manipulation: *
				***************************/

				// Generating a new instance:
				virtual TVRemitter<T> *newInstance(TVObject<T> *obj) const {
					// Using the constructor:
					TVRELabelThDecayMax<T> *re = 
						new TVRELabelThDecayMax<T>(obj,_labelsGroup,_th,_reqSals);

					// Returning:
					return re;
				}

				/*********************
				* Abstract methods: *
				*********************/

				// The thresholding function:
				virtual bool threshold(const T *features) const {
					// Computing the maximum value and thresholding:
					return features[vec_max(features,getDim())]>_th;
				}
			};

		} // namespace Labelling
	} // namespace Extensions
} // namespace TVF
#endif //ndef _TVRELABELTHDECAYMAX_H_
