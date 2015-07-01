
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

#ifndef _TVRELABELLINGGROUP_H_
#define _TVRELABELLINGGROUP_H_

/*
 *  This class represents a group of labels that permits the generation
 * of labels and the management of equivalence classes between them.
 */

// All this in the TVF namespace:
namespace TVF {
	namespace Extensions {
		namespace Labelling {

			// Usings:
			using namespace TVF::DataStructures;
			using namespace stdext;

			// The class:
			template <class T>
			class TVRELabellingGroup {
			public:
				/**********
				* Types: *
				**********/

				// A map type:
				typedef hash_map<vector_ind_t,TVSpace<T>*> _hash_t;

				// A key/value pair:
				typedef pair<vector_ind_t,TVSpace<T>*> _hash_pair;

				// An hash iterator:
				typedef typename _hash_t::iterator _hash_iterator;

			protected:
				/***************
				* Attributes: *
				***************/

				// The structure that contains the equivalence classes:
				TVDsEquivalenceSet<vector_ind_t> *_eqSet;

				// Actually free label:
				vector_ind_t _freeLabel;

			public:
				/********************************
				* Constructors and destructor: *
				********************************/

				// Default constructor:
				TVRELabellingGroup() : _eqSet(NULL), _freeLabel(1) {
					// Allocating the eqSet:
					_eqSet =  new TVDsEquivalenceSet<vector_ind_t>(0);
				}

				// Destructor:
				~TVRELabellingGroup() {
					// Deallocating the eqSet:
					cleanEqSet();
				}

				/*****************
				* Custom tools: *
				*****************/

				// Getter for my class:
				vector_ind_t getClass(vector_ind_t label) {
					// The real class' label:
					return getEqSet()->getClass(label);
				}

				// Cleaning of the eqSet:
				void cleanEqSet() {
					// Removing the eqSet data structure:
					if (_eqSet) {
						// Deleting:
						delete _eqSet;
						_eqSet = NULL;
					}

					// Resetting the labels generator:
					_freeLabel = 1;
				}

				// Getter for the eqSet:
				TVDsEquivalenceSet<vector_ind_t> *getEqSet() {
					// Checking:
					if (!_eqSet) {
						// Creating an empty one:
						_eqSet = new TVDsEquivalenceSet<vector_ind_t>(0);
						_freeLabel = 1;
					}

					// Returning:
					return _eqSet;
				}
				
				// Getter for a free label:
				vector_ind_t getFreeLabel() {
					// Returning:
					return _freeLabel++;
				}

				// Splitting a space with labelled remitters into a vector of spaces:
				_hash_t *split(TVSpace<T> *sp,vector_dim_t lblRemIndex,bool move=true) {
					// Iterating on the content of the space:
					TVObject<T>::iterator_t *iter = sp->getIterator();
					TVRELabel<T> *re;
					_hash_t *hash = new _hash_t;
					TVSpace<T> *space;
					_hash_iterator i;
					vector_ind_t cl;

					// Iterating:
					while (iter->hasActual()) {
						// Getting the remtter with the label:
						re = dynamic_cast<TVRELabel<T>*>(iter->getActual()->getRemitter(lblRemIndex));
						if (re) {
							// Removing outliers:
							cl = re->getClass();
							if (cl==0) {
								// Checking the case:
								if (move) {
									// Removing the object:
									sp->remove(re->getObject());

									// Next one:
									iter->reset();
								}
								else {
									// Next one:
									iter->next();
								}

								// Next iteration:
								continue;
							}

							// Getting the space for this label:
							i = hash->find(cl);
							if (i!=hash->end()) {
								// The space:
								space = i->second;
							}
							else {
								// No space available:
								space = NULL;
							}

							// Creating a new space if required:
							if (!space) {
								// Allocating it:
								space = sp->newInstance();

								// Adding the space to the hash map:
								hash->insert(_hash_pair(cl,space));
							}

							// Moving (or copying) the object:
							if (move) {
								// Moving the object to me:
								space->move(re->getObject(),sp);

								// Next object:
								iter->reset();
							}
							else {
								// Copying the object to me:
								space->add(re->getObject()->clone());

								// Next object:
								iter->next();
							}
						}
					}

					// Cleaning:
					delete iter;

					// Returning:
					return hash;
				}
			};

		} // namespace Labelling
	} // namespace Extensions
} // namespace TVF

#endif //ndef _TVRELABELLINGGROUP_H_
