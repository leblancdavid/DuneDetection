
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

#ifndef _TVMARCHSEQUENTIAL_H_
#define _TVMARCHSEQUENTIAL_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {
		namespace Marching {

			// The usings:
			using namespace TVF::Math;
			using namespace TVF::DataStructures::Spaces;
			using namespace TVF::DataStructures::Spaces::Filters;

			/*
			 *  This class represents a sequential implementation
			 * of the marching process.
			 */
			template <class T>
			class TVMarchSequential : public TVMarch<T> {
			protected:
				/***************
				 * Attributes: *
				 ***************/

				// The front space:
				TVSpace<T> *_front;

			public:
				/********************************
				 * Constructors and destructor: *
				 ********************************/

				// Default constructor:
				TVMarchSequential(TVProcess<T> *pro,ITVSaliencyMaximizer<T> *maximizer,T dt) : 
				  TVMarch<T>(pro,maximizer,dt), _front(NULL) {}

				// Destructor:
				~TVMarchSequential() {
					// Deleting the spaces:
					clean();
				}

				// Cleaning:
				void clean() {
					// Cleaning the spaces:
					if (_front) {
						// Deleting:
						delete _front;
						_front = NULL;
					}
				}

				/*************************************
				 * Abstract methods' implementation: *
				 *************************************/

				// Marching by iterative expansions:
				virtual TVSpace<T> *march(TVSpace<T> *front) {
					// Cleaning:
					clean();

					// Creating the initial front and structure:
					_front = front;
					TVSpace<T> *structure = NULL;

					// Expanding iteratively:
					while(_front->getSize()>0) {
						// An expansion step:
						structure = expand(structure);
					}

					// The front is useless now:
					delete _front;
					_front = NULL;

					// Returning:
					return structure;
				}

				// Generating new points near to the front:
				virtual TVSpace<T> *expand(TVSpace<T> *structure) {
					// Checking the front:
					if (!_front) return NULL;

					// Initializing:
					TVObject<T>::iterator_t *i, *j, *k;
					TVObjectExpand<T> *obj;
					TVObject<T> *obj2;
					TVSpace<T>	*nfront = _front->newInstance(),
								*tmp = new TVSpVector<T>(_front->getDim());
					if (!structure) structure=_front->newInstance();

					// Iterating on the front points:
					for (i=_front->getIterator(); i->hasActual(); i->reset()) {
						// Expanding this front point:
						obj = dynamic_cast<TVObjectExpand<T> *>(i->getActual());
						if (obj) {
							// Expanding:
							obj->expand(_pro,tmp,_maximizer,_dt);

							//  Adding the objects to the new front if they are not
							// too close one with each other:
							for(j=tmp->getIterator(); j->hasActual(); j->reset()) {
								// Checking the closeness:
								obj2 = j->getActual();
								
								// Removing all the found objects from the front:
								k = nfront->range(obj2->getPos(),_dt);
								if (k->hasActual()) {
									// Removing all bad objects from nfront:
									for (;k->hasActual(); k->reset()) {
										// Removing the object:
										nfront->remove(k->getActual());
									}
									delete k;

									// Removing also the conflicting object (using it in the structure):
									structure->move(obj2,tmp);
								}
								else {
									// Checking if obj2 has encountered the structure:
									delete k;
									k = structure->range(obj2->getPos(),_dt);
									if (k->hasActual()) {
										// Moving to the structure:
										structure->move(obj2,tmp);
									}
									else {
										// Adding the new front object:
										nfront->move(obj2,tmp);
									}
									delete k;
								}
							}

							// Cleaning:
							delete j;

							// Moving the object to the structure space:
							structure->move(obj,_front);
						}
						else {
							// Removing the object:
							_front->remove(i->getActual());
						}
					}

					// Cleaning:
					delete tmp;
					delete i;

					// Substituting the front:
					delete _front;
					_front = nfront;

					// Returning:
					return structure;
				}

			protected:
				/********************
				 * Protected tools: *
				 ********************/
			};

		} // namespace Marching
	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVMARCHSEQUENTIAL_H_
