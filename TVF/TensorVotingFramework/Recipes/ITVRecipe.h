
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

#ifndef _ITVRECIPE_H_
#define _ITVRECIPE_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		/*
		 *  This interface represent a voting algorithm recipe.
		 */
		template <class T>
		class ITVRecipe {
		public:
			// Destructor:
			virtual ~ITVRecipe() {}

			/*****************
			 * Recipe usage: *
			 *****************/

			// Running the recipe:
			virtual void run(TVProcess<T> *process)=0;

			// Resetting the recipe:
			virtual void reset() {};
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _ITVRECIPE_H_
