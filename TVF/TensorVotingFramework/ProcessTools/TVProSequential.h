
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

#ifndef _TVPROSEQUENTIAL_H_
#define _TVPROSEQUENTIAL_H_

// All this in the plain TVF namespace:
namespace TVF {
	namespace ProcessTools {

		/*
		 *  This concrete process is the simplest one that can be implemented
		 * and allows to manage the voting sequentially.
		 */
		template <class T>
		class TVProSequential : public TVProcess<T> {
		public:
			/********************************
			 * Constructors and destructor: *
			 ********************************/

			// Default constructor:
			TVProSequential(T *data,vector_ind_t N,TVFactory<T> *fac,ITVRecipe<T> *recipe) :
				TVProcess<T>(data,N,fac,recipe) {}

			/************************************
			 * Implementation of the interface: *
			 ************************************/

			// A single voting iteration:
			virtual void votingPass() const {
				// Iterators:
				TVObject<T>::iterator_t *i,*range;
				TVObject<T> *obj,*obj2;

				// Iterating over objects:
				for (i=_space->getIterator(); i->hasActual(); i->next()) {
					// Initialization:
					i->getActual()->init();
				}

				// Iterating over objects:
				for (i->reset(); i->hasActual(); i->next()) {
					// Computing a range query over this i-th target receiver:
					obj = i->getActual();
					range = _space->range(obj->getPos());

					// Iterating over the range:
					for (; range->hasActual(); range->next()) {
						// Generating a vote from the object in the range to the actual object:
						obj2 = range->getActual();
						if (obj!=obj2) {
							// Real voting:
							obj->vote(obj2);
						}
					}

					// Freeing memory:
					delete range;
				}

				// Iterating over objects:
				for (i->reset(); i->hasActual(); i->next()) {
					// Initialization:
					i->getActual()->update();
				}

				// Deleting the iterator:
				delete i;
			}

			// Voting on a single target point:
			virtual TVObject<T> *voteTarget(	T *p,
												TVObject<T> *dest=NULL,
												TVSpace<T> *space=NULL) const {
				// The space:
				if (space==NULL) {
					// Using my own space:
					space = _space;
				}

				// The destination object:
				if (dest==NULL) {
					// Creating a new one:
					dest = getFactory()->createObject(p)
						->setPosAggregated(false);
				}
				else {
					// Setting the new position:
					dest->setPos(p);
				}

				// The iterator:
				TVObject<T>::iterator_t *iter = space->range(p);

				// Voting on this object:
				dest->init();
				for (; iter->hasActual(); iter->next()) {
					// Voting:
					dest->vote(iter->getActual());
				}
				dest->update();

				// Deleting the iterator:
				delete iter;

				// Returning the object:
				return dest;
			}

			// Voting on specified target points:
			virtual TVSpace<T> *voteTarget(	ITVIterator<T*> *pts,
											TVSpace<T> *dest=NULL,
											TVSpace<T> *space=NULL) const {
				// The space:
				if (space==NULL) {
					// Using my own space:
					space = _space;
				}

				// Preparing the space:
				if (dest==NULL) {
					// Creating a new space:
					dest = getFactory()->createSpace(NULL,0,space->getDim());
				}

				// Iterating on points:
				for (pts->reset(); pts->hasActual(); pts->next()) {
					// Adding the new object obtained:
					dest->add(voteTarget(pts->getActual(),NULL,space));
				}

				// Compacting the destination space:
				dest->compact();

				// Returning it:
				return dest;
			}

			// Voting and saliency computation on objects:
			// WARNING: The given buffer is supposed to be sized correctly!
			virtual T *computeSaliency(T *dest=NULL) const {
				// Allocating the destination buffer:
				if (!dest) {
					// Allocation:
					dest = new T[_space->getSize()*_computeTotalSalsNum(_space)];
				}

				// Preparing the visitor and the iterator:
				ITVVisitObject<T> *visitor = new TVVOSaliency<T>(dest);

				// Visiting objects:
				visit(visitor);

				// Cleaning and returning:
				delete visitor;
				return dest;
			}

			// Voting and saliency computation on given points:
			virtual T *computeSaliency(ITVIterator<T*> *pts,T *dest=NULL) const {
				// Allocation?
				if (dest==NULL) {
					// Allocation:
					dest = new T[pts->getNumel()*_computeTotalSalsNum(_space)];
				}

				// Allocating an object:
				TVObject<T> *obj = getFactory()->createObject(NULL);

				// The visitor:
				TVVOSaliency<T> *visitor = new TVVOSaliency<T>(dest);
				visitor->init();

				// Iterating on points:
				for (pts->reset(); pts->hasActual(); pts->next()) {
					// Voting on this point:
					voteTarget(pts->getActual(),obj);

					// Getting the saliency from this object:
					visitor->visit(obj);
				}

				// Finalization:
				visitor->finalize();

				// Cleaning and returning:
				delete obj;
				delete visitor;
				return dest;
			}

			// A more general visitor:
			void visit(ITVVisitObject<T> *visitor,
				typename TVObject<T>::iterator_t *i=NULL) const {
				// Local checks:
				bool mustDeleteIter = i==NULL;
				if (visitor==NULL) return;

				// The iterator:
				if (i==NULL) {
					// Checking the space:
					if (_space) {
						// Getting the iterator:
						i = _space->getIterator();
					}
					else {
						// An error:
						return;
					}
				}

				// Initialization:
				visitor->init();

				// Iterating:
				for (; i->hasActual(); i->next()) {
					// Visiting:
					visitor->visit(i->getActual());
				}

				// Deleting the iterator:
				if (mustDeleteIter) delete i;

				// Finalization:
				visitor->finalize();
			}

			// Computes the number of T elements in a saliency vector for a single object:
			vector_dim_t computeTotalSalsNum() const {
				// Using the appropriate method:
				return _computeTotalSalsNum(_space);
			}

		protected:
			/**********************
			 * Protected members: *
			 **********************/

			// Computes the number of T elements in a saliency vector for a single object:
			static vector_dim_t _computeTotalSalsNum(TVSpace<T> *space) {
				// The masking of remitters:
				long mask = TVParameters<T>::getInstance()->getParameterInt(TVPARI_REM_MASK);

				// Getting some infos:
				vector_dim_t totalSalsNum = 0, i,Nrems;

				// Getting one object:
				TVObject<T>::iterator_t *iter = space->getIterator();
				TVObject<T> *obj = iter->getActual();
				delete iter;

				// Getting Nrems:
				Nrems = obj->getNrems();

				// Computing the number of saliencies:
				for (i=0; i<Nrems; i++) {
					// Checking if this remitter is used:
					if ((mask >> i) & 0x01) {
						// Adding the number of saliencies:
						totalSalsNum += obj->getRemitter(i)->getSaliencyNum();
					}
				}

				// Returning:
				return totalSalsNum;
			}
		};

	} // namespace ProcessTools
} // namespace TVF

#endif //ndef _TVPROSEQUENTIAL_H_
