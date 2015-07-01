
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

#ifndef _ITVRECPLAIN_H_
#define _ITVRECPLAIN_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		/*
		 *  This interface represent a voting algorithm recipe.
		 */
		template <class T>
		class TVRecPlain : public ITVRecipe<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// Number of iterations:
			short _iters;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVRecPlain(short iters=1) : _iters(iters) {}


			/*****************
			 * Recipe usage: *
			 *****************/

			// Running the recipe:
			virtual void run(TVProcess<T> *process) {
				// Using the actually set of parameters:
				for (short i=0; i<_iters; i++) {
					// A voting pass:
					process->votingPass();
				}
			}

			/**************
			 * Accessors: *
			 **************/

			// Setter and getter for the iterations number:
			TVRecPlain<T> *setIters(short iters=2) {
				// Setting:
				_iters = iters;

				// Returning:
				return this;
			}
			short getIters() const {
				// Returning:
				return _iters;
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _ITVRECPLAIN_H_
