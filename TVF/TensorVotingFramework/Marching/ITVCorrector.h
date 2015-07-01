
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

#ifndef _ITVCORRECTOR_H_
#define _ITVCORRECTOR_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This interface represents a corrector component that corrects the position of
			 * voted objects.
			 */

			template <class T>
			class ITVCorrector {
			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default destructor:
				virtual ~ITVCorrector() {}
			
				/************************
				 * The virtual methods: *
				 ************************/

				/*
				 *  Correction of the points positions in a space:
				 * IN:
				 *  pro:	The process with the space to be modified.
				 *  ord:	The number of compressed dimensions fo the
				 *			manifold of interest.
				 */
				virtual void correct(TVProcess<T> *pro,vector_dim_t ord)=0;
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _ITVCORRECTOR_H_
