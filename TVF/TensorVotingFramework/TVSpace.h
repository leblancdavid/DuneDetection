
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

#ifndef _TVSPACE_H_
#define _TVSPACE_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	 *  This abstract class represents a general space data
	 * structure where the objects can be saved.
	 */

	// Namespaces used:
	using namespace TVF::Math;

	template <class T>
	class TVSpace {
	protected:
		/***************
		 * Attributes: *
		 ***************/

		// My dimensionality:
		vector_dim_t _dim;

		// Padding to align the memory space:
		vector_dim_t _pad;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// Base constructor for a space:
		TVSpace(vector_dim_t dim=-1) : _dim(dim) {
			// Setting the default:
			if (dim<=0) {
				// Using the parameters value:
				_dim = (vector_dim_t)TVParameters<T>::getInstance()
					->getParameterInt(TVPARI_DIM);
			}
		}

		// Base destructor:
		virtual ~TVSpace() {}

		// Generating a new instance:
		virtual TVSpace<T> *newInstance(vector_dim_t dim=0) const=0;

		/***************************
		 * Structure manipulation: *
		 ***************************/

		// Getter for the dimensionality:
		vector_dim_t getDim() const {
			// Returning:
			return _dim;
		}

		// Getter for the number of objects:
		virtual vector_ind_t getSize() const=0;

		// Adding an object:
		virtual void add(TVObject<T> *obj)=0;

		// Adding a whole space:
		virtual void add(TVSpace<T> *sp) {
			// Getting an iterator:
			TVObject<T>::iterator_t *iter = sp->getIterator();

			// Iterating on objects:
			for (; iter->hasActual(); iter->next()) {
				// Adding the object:
				add(iter->getActual()->clone());
			}
			delete iter;

			// Compactifying myself:
			compact();
		}

		// Moving a single object from another space:
		virtual void move(TVObject<T> *obj,TVSpace<T> *sp) {
			// Extracting the object:
			TVObject<T> *o = sp->extract(obj);

			// Adding it to me:
			if (o) add(o);
		}

		// Moving a whole space:
		virtual void move(TVSpace<T> *sp) {
			// Getting an iterator:
			TVObject<T>::iterator_t *iter = sp->getIterator();

			// Iterating on objects:
			for (; iter->hasActual(); iter->next()) {
				// Adding the object:
				add(iter->getActual());
			}
			delete iter;

			// Extracting all the objects:
			sp->empty();

			// Compactifying myself:
			compact();
		}

		// Extracting an object:
		virtual TVObject<T> *extract(TVObject<T> *obj)=0;

		// Deleting an object:
		virtual void remove(TVObject<T> *obj) {
			// Extracting the object:
			TVObject<T> *o = extract(obj);

			// Deleting:
			if (o) delete o;
		}

		// Updating an object when its position is externally changed:
		virtual void update(TVObject<T> *obj)=0;

		// Extracting all the objects:
		virtual void empty()=0;

		// Cleaning (removing the objects):
		virtual void clean()=0;

		// Resetting all objects:
		virtual void reset()=0;

		// Compactification (reducing the used memory):
		virtual void compact() {}

		// Getting the iterator over all the objects:
		virtual typename TVObject<T>::iterator_t *getIterator() const=0;

		// Getting a range of objects:
		// Radius -1 means "use the default one"
		virtual typename TVObject<T>::iterator_t *range(const T *center, T radius=-1) const=0;
	};

} // namespace TVF

#endif //ndef _TVSPACE_H_
