
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

#ifndef _TVVORPLOT_H_
#define _TVVORPLOT_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Visitors {

			/*
			*  This class represent a general visitor for the remitter hierarchy.
			*/
			template <class T>
			class TVVORPlot : public TVVORemitters<T> {
			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVVORPlot(ITVTerminal<T> *term) : 
				TVVORemitters<T>(new TVVRPlot<T>(term)) {}

				/************************************
				* Implementation of the interface: *
				************************************/

				// Initialization:
				virtual void init() {
					// Delegating:
					dynamic_cast<TVVRPlot<T>*>(_visitor)->getTerminal()->init();
				}

				// Finalization:
				virtual void finalize() {
					// Delegating:
					dynamic_cast<TVVRPlot<T>*>(_visitor)->getTerminal()->finalize();
				}

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the terminal:
				ITVTerminal<T> *getTerminal() const {
					// Returning:
					return _visitor->getTerminal();
				}
				TVVORPlot<T> *setTerminal(ITVTerminal<T> *term) {
					// Setting:
					_visitor->setTerminal(term);

					// Returning:
					return this;
				}
			};

		} // namespace Visitors
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVVORPLOT_H_
