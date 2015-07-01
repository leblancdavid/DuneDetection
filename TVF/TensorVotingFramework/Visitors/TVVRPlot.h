
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

#ifndef _TVVRPLOT_H_
#define _TVVRPLOT_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Visitors {

			/*
			*  This class represent a general visitor for the remitter hierarchy.
			*/
			template <class T>
			class TVVRPlot : public ITVVisitRemitter<T> {
			protected:
				/***************
				* Attributes: *
				***************/

				// My terminal:
				ITVTerminal<T> *_term;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVVRPlot(ITVTerminal<T> *term) : _term(term) {}

				// Destructor:
				virtual ~TVVRPlot() {}

				/************************************
				* Implementation of the interface: *
				************************************/

				// Initialization:
				virtual void init(TVObject<T> *obj) {
					// Delegating:
					_term->initObjectSection(obj);
				}

				// The visit method:
				virtual void visit(TVRemitter<T> *rem) {
					// Delegating:
					_term->plotRemitter(rem);
				}

				// Finalization:
				virtual void finalize() {
					// Delegating:
					_term->finalizeObjectSection();
				}

				/**************
				* Accessors: *
				**************/

				// Getter and setter for the terminal:
				ITVTerminal<T> *getTerminal() const {
					// Returning:
					return _term;
				}
				TVVRPlot<T> *setTerminal(ITVTerminal<T> *term) {
					// Setting:
					_term = term;

					// Returning:
					return this;
				}
			};

		} // namespace Visitors
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVVRPLOT_H_
