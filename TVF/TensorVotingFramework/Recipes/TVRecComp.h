
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

#ifndef _ITVRECCOMP_H_
#define _ITVRECCOMP_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		/*
		 *  This interface represent a voting algorithm recipe.
		 */
		template <class T>
		class TVRecComp : public ITVRecipe<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The recipes sequence:
			vector<ITVRecipe<T> > *_recipes;

			// The iterations:
			short _iters;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVRecComp(vector<ITVRecipe<T> > *recipes,short iters=1) : 
			  _recipes(recipes), _iters(iters) {}

			// Destructor:
			~TVRecComp() {
				// Deleting sub-recipes:
				_cleanRecipes();
			}

			/*****************
			 * Recipe usage: *
			 *****************/

			// Running the recipe:
			virtual void run(TVProcess<T> *process) {
				// Executing all:
				if (_recipes) {
					// The iterator:
					vector<ITVRecipe<T> >::iterator i;

					// The required iterations:
					for (int i=0; i<_iters; i++) {
						// Iterating on recipes:
						for (i = _recipes->begin(); i!=_recipes->end(); i++) {
							// Resetting:
							*i->run(process);
						}
					}
				}
			}

			// Resetting the recipe:
			virtual void reset() {
				// Resetting all:
				if (_recipes) {
					// Iterating on recipes:
					vector<ITVRecipe<T> >::iterator i;
					for (i = _recipes->begin(); i!=_recipes->end(); i++) {
						// Resetting:
						*i->reset();
					}
				}
			}

			/**************
			 * Accessors: *
			 **************/

			// Setter and getter for the iterations number:
			TVRecComp<T> *setIters(short iters=2) {
				// Setting:
				_iters = iters;

				// Returning:
				return this;
			}
			short getIters() const {
				// Returning:
				return _iters;
			}

			// Setter and getter for the recipes:
			TVRecComp<T> *setRecipes(vector<ITVRecipe<T> > *recipes) {
				// Cleaning the old recipes:
				_cleanRecipes();

				// Setting the new ones:
				_recipes = recipes;

				// Returning:
				return this;
			}
			const vector<ITVRecipe<T> > *getRecipes() const {
				// Returning:
				return _recipes;
			}

		protected:
			/**********************
			 * Protected methods: *
			 **********************/

			// Cleaning the recipes vector:
			void _cleanRecipes() {
				// Deleting sub-recipes:
				if (_recipes) {
					// Iterating on recipes:
					vector<ITVRecipe<T> >::iterator i;
					for (i = _recipes->begin(); i!=_recipes->end(); i++) {
						// Resetting:
						delete *i;
					}

					// Also the vector:
					delete _recipes;
					_recipes = NULL;
				}
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _ITVRECCOMP_H_
