
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

#ifndef _TVRemitter_H_
#define _TVRemitter_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	*  A remitter is an element stored in an object. A remitter can
	* generate a desne voting field around the object and can receive
	* votes from other remitters of the same kind. Examples of remitters
	* are the T1 and T2 tensorial votes receiver and emitter.
	*/

	// Namespaces used:
	using namespace TVF::Math;

	// This classes must be known by the remitter:
	// - The owning object:
	template <class T>
	class TVObject;
	// - The visitor for the remitter:
	template <class T>
	class ITVVisitRemitter;


	// The remitter abstract class:
	template <class T>
	class TVRemitter {
	protected:
		/***************
		 * Attributes: *
		 ***************/

		// My object:
		TVObject<T> *_obj;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// Default constructor:
		TVRemitter(TVObject<T> *obj) : _obj(obj) {}

		// Destructor:
		virtual ~TVRemitter() {}

		/***************************
		 * Structure manipulation: *
		 ***************************/

		// Generating a new instance:
		virtual TVRemitter<T> *newInstance(TVObject<T> *obj) const=0;

		// Getting my object:
		TVObject<T> *getObject() const {
			// Returning:
			return _obj;
		};

		// Accepting a visitor (virtual to allow double dispatching):
		virtual void accept(ITVVisitRemitter<T> *visitor) {
			// The visitor can be used here so that the double dispatching is executed:
			visitor->visit(this);
		}

		// Getter for the dimensionality:
		vector_dim_t getDim() const {
			// Returning:
			return _obj!=NULL? _obj->getDim(): 0;
		}

		/*************************************
		 * Managment of the decay functions: *
		 *************************************/

		// Setting the decay function:
		virtual void setDecay(ITVDecayFun<T> *func)=0;

		// Getting the decay function:
		virtual ITVDecayFun<T> *getDecay() const=0;

		/*****************
		 * Voting tools: *
		 *****************/

		// Initialization of the voting process:
		virtual void init() {}

		// Voting:
		virtual void vote(TVRemitter<T> *rem)=0;
		// The << operator do the same (but returns the new vote):
		TVRemitter<T> &operator <<(TVRemitter<T> *rem) {
			// Voting:
			vote(rem);

			// Returning:
			return *this;
		}
		// The >> operator returns the voter:
		TVRemitter<T> &operator >>(TVRemitter<T> *rem) {
			// Voting:
			rem->vote(this);

			// Returning:
			return *this;
		}

		// Updating:
		virtual void update() {}

		// Resetting:
		virtual void reset() {}

		// Computing the saliencies:
		virtual T *computeSals(T *buff=NULL) const=0;

		// Sayng the number of saliency values generated:
		virtual vector_dim_t getSaliencyNum() const=0;

		// Printing:
		virtual void fprint(FILE *fp=stderr) {}
	};

} // namespace TVF

#endif //ndef _TVRemitter_H_
