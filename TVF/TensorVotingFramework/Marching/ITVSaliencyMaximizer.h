
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

#ifndef _ITVSALIENCYMAXIMIZER_H_
#define _ITVSALIENCYMAXIMIZER_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			/*
			 *  This interface defines a general saliency maximizer algorithm.
			 */

			template <class T>
			class ITVSaliencyMaximizer {
			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default destructor:
				virtual ~ITVSaliencyMaximizer() {}

				/************************
				 * The virtual methods: *
				 ************************/

				/*
				 *  This method allows to maximize the saliency starting from a point p0 and moving on the
				 * the ord compressed dimensions of the given tensor tnsd.
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
					vector_dim_t ord,const T *p1,T *dest=NULL)=0;
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _ITVSALIENCYMAXIMIZER_H_
