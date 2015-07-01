
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

#ifndef _ITVVOTINGSYSTEM_H_
#define _ITVVOTINGSYSTEM_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	 *  This is the principal facade of the system, this interface
	 * describes the access to the voting system. Only new data are
	 * passed through the calls to the metods of this interface (like
	 * target points) becaus it is supposed that the system has stored
	 * the objects with teir coordinates, the space, the parameters etc etc.
	 */

	template <class T>
	class ITVVotingSystem {
	public:
		// Destructor:
		virtual ~ITVVotingSystem() {}

		/*****************
		 * Voting tools: *
		 *****************/

		// A single voting iteration:
		virtual void votingPass() const=0;

		// Voting with the internal recipe:
		virtual void vote()=0;

		// Voting on a single target point:
		virtual TVObject<T> *voteTarget(	T *p,
											TVObject<T> *dest=NULL,
											TVSpace<T> *space=NULL) const=0;

		// Voting on specified target points:
		virtual TVSpace<T> *voteTarget(	ITVIterator<T*> *pts,
										TVSpace<T> *dest=NULL,
										TVSpace<T> *space=NULL) const=0;

		/*******************
		 * Saliency tools: *
		 *******************/

		//  Computes the number of T elements in a saliency
		// vector for a single object:
		virtual vector_dim_t computeTotalSalsNum() const=0;

		// Saliency computation on objects in the space:
		virtual T *computeSaliency(T *dest=NULL) const=0;

		// Voting and saliency computation on given points:
		virtual T *computeSaliency(ITVIterator<T*> *pts,T *dest=NULL) const=0;

		/*******************
		 * Visiting tools: *
		 *******************/

		// Reinitialization with previously given data:
		virtual void reInit()=0;

		// Visiting the structure:
		// i==NULL means "use the objects in the space".
		virtual void visit(ITVVisitObject<T> *visitor,
				typename TVObject<T>::iterator_t *i=NULL) const=0;

		/**************
		 * Accessors: *
		 **************/

		// Getter for the parameters:
		virtual TVParameters<T> *getParameters() const=0;

		// Getter and setter for the space:
		virtual TVSpace<T> *getSpace() const=0;
		virtual void setSpace(TVSpace<T> *sp)=0;
	};

} // namespace TVF

#endif //ndef _ITVVOTINGSYSTEM_H_
