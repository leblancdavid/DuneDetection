
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

#ifndef _TVPROCESS_H_
#define _TVPROCESS_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		// Forward for the recipe's interface:
		template <class T>
		class ITVRecipe;

		/*
		 *  This abstract class represents a voting process that allows to manage the
		 * voting itself. This abstraction allows to get different voting process
		 * types like sequential single threaded and single process voting, threaded
		 * (single machine parallel) voting and distributed (multiprocess and potentially
		 * multi-machine voting using a cluster). This class implements also the facade
		 * interface of the subsystem ITVVotingSystem.
		 */
		template <class T>
		class TVProcess : public ITVVotingSystem<T> {
		protected:
			/***************
			 * Attributes: *
			 ***************/

			// The factory:
			TVFactory<T> *_factory;

			// The recipe:
			ITVRecipe<T> *_recipe;

			// The space:
			TVSpace<T> *_space;

			// Data:
			T *_data;

			// Number of elements:
			vector_ind_t _N;

		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVProcess(T *data,vector_ind_t N,TVFactory<T> *fac,ITVRecipe<T> *recipe) :
				_factory(fac), _recipe(recipe), _data(data), _N(N), _space(NULL) {
					// Initialization:
					_init();
			}

			// Destructor:
			~TVProcess() {
				// Deleting all:
				_dispose();
			}

			/************************************
			 * Implementation of the interface: *
			 ************************************/

			// Voting with recipes:
			virtual void vote() {
				// Using the recipe:
				executeRecipe(NULL);
			}

			// Reinitialization with previously given data:
			virtual void reInit() {
				// By now this is the same as _init:
				_init();
			}

			/**********************
			 * My public methods: *
			 **********************/

			// Recipe execution:
			void executeRecipe(ITVRecipe<T> *rec) {
				// Checing the recipe:
				if (!rec) rec=_recipe;

				// Check the recipe:
				if (rec) {
					// Init:
					_votingCycle_init();

					// Recipe execution:
					rec->run(this);

					// Preparing for the next execution:
					rec->reset();

					// Finalize:
					_votingCycle_finalize();
				}
			}

			/**************
			 * Accessors: *
			 **************/

			// Getter for the factory:
			TVFactory<T> *getFactory() const {
				// Returning:
				return _factory;
			}

			// Getter for the parameters:
			TVParameters<T> *getParameters() const {
				// Returning:
				return TVParameters<T>::getInstance();
			}

			// Getter and setter for the recipe:
			ITVRecipe<T> *getRecipe() const {
				// Returning:
				return _recipe;
			}
			void setRecipe(ITVRecipe<T> *recipe) {
				// Setting:
				if (_recipe) delete _recipe;
				_recipe = recipe;
			}

			// Getter and setter for the space:
			TVSpace<T> *getSpace() const {
				// Returning:
				return _space;
			}
			void setSpace(TVSpace<T> *sp) {
				// Setting:
				_space = sp;
			}

		protected:
			/*************************
			 * My protected methods: *
			 *************************/

			// Initialization of the recipe execution:
			virtual void _votingCycle_init() {}

			// Finalization of the recipe execution:
			virtual void _votingCycle_finalize() {}

			// Initializing the process object:
			virtual void _init() {
				// Checking:
				if (_factory && _data) {
					// Creating the space with objects in it:
					if (_space) delete _space;
					_space = _factory->createSpace(_data,_N,
						(vector_dim_t)getParameters()->getParameterInt(TVPARI_DIM));
				}
			}

			// Deleting all:
			virtual void _dispose() {
				// Deleting single objects:
				if (_factory) delete _factory;
				if (_recipe) delete _recipe;
				if (_space) delete _space;
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVPROCESS_H_
