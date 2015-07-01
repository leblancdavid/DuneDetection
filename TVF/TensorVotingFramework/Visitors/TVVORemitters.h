
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

#ifndef _TVVOREMITTERS_H_
#define _TVVOREMITTERS_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Visitors {

			/*
			*  This class represent a general visitor for the remitter hierarchy.
			*/
			template <class T>
			class TVVORemitters : public ITVVisitObject<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// The visitor for remitters:
				TVVRPlot<T> *_visitor;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVVORemitters(TVVRPlot<T> *visitor) : _visitor(visitor) {}

				// Destructor:
				~TVVORemitters() {
					// Deleting the visitor:
					delete _visitor;
				}

				/************************************
				* Implementation of the interface: *
				************************************/

				// The visit method:
				virtual void visit(TVObject<T> *obj) {
					// The masking of remitters:
					long mask = TVParameters<T>::getInstance()->getParameterInt(TVPARI_REM_MASK);

					// Getting infos:
					long N = obj->getNrems();
					vector_dim_t i;

					// Initializing the visiting:
					_visitor->init(obj);

					// Iterating on remitters:
					for (i=0; i<N; i++) {
						// Checking if this remitter is used:
						if ((mask >> i) & 0x01) {
							// Visiting:
							_visitor->visit(obj->getRemitter(i));
						}
					}

					// Finalization:
					_visitor->finalize();
				}
			};

		} // namespace Visitors
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVVOREMITTERS_H_
