
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

#ifndef _TVOBJECT_H_
#define _TVOBJECT_H_

// Local macros:
#define _TVO_DELEGATE_REMITTERS(__action) \
		{	/* A local block: */ \
			/* Iterating on the remitters: */ \
			if (_rems!=NULL && _Nrems>0) { \
				/* Getting the mask: */ \
				/* NOTICE: This constrain the number of remitters up to a max of 32. */ \
				long mask = TVParameters<T>::getInstance()->getParameterInt(TVPARI_REM_MASK); \
				/* Iterating: */ \
				for (vector_dim_t i=0; i<_Nrems; i++) { \
					/* Checking the mask: */ \
					if ((mask >> i) & 0x01) { \
						/* Delegating: */ \
						__action; \
					} \
				} \
			} \
		}

// All this in the plain TVF namespace:
namespace TVF {

	/*
	 *  This class represents an object of the space, in it an ordered set of
	 * remitters is encoded and used during the voting process.
	 */

	// Namespaces used:
	using namespace TVF::Math;

	// Class definition:
	template <class T>
	class TVObject {
	public:
		/*****************
		 * Public types: *
		 *****************/

		// A set of particular iterators:
		typedef typename ITVIterator<TVObject<T>*> iterator_t;

	protected:
		/***************
		 * Attributes: *
		 ***************/

		// My dimensionality:
		vector_dim_t _dim;

		// My number of remitters:
		unsigned char _Nrems;

		// Is the position aggregated?
		bool _posAggregated;

		// My position (aggregated depending on the _posAggregated attribute, or else composite):
		T *_pos;

		// My remitters:
		TVRemitter<T> **_rems;

		// Number of received votes:
		vector_dim_t _Nvotes;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		/*
		 *  This constructor allows to generate a new object without
		 * remitteres, generally this is used to create the prototypical object
		 * in the factory.
		 */
		TVObject(vector_dim_t dim=0,bool posAggregated=true) 
				: _rems(NULL), _Nrems(0), _pos(NULL), _dim(dim),
					_posAggregated(posAggregated), _Nvotes(0) {
			// Setting the default:
			if (dim<=0) {
				// Using the parameters value:
				_dim = (vector_dim_t)TVParameters<T>::getInstance()
					->getParameterInt(TVPARI_DIM);
			}
		}

		// A cloning method:
		virtual TVObject<T> *newInstance(T *pos) const {
			// Creating an empty object:
			TVObject<T> *obj = new TVObject(_dim,_posAggregated);

			// Creating a new array of remitters:
			TVRemitter<T> **rems;

			// Cloning the remitters:
			if (_rems!=NULL && _Nrems>0) {
				rems = new TVRemitter<T>*[_Nrems];
				// Iterating:
				for (vector_dim_t i=0; i<_Nrems; i++) {
					// Cloning a single remitter:
					rems[i] = _rems[i]->newInstance(obj);
				}
			}
			else {
				// No rems:
				rems = NULL;
			}

			// Setting the remiters:
			obj->setRemitters(rems,_Nrems);

			// Setting the position:
			obj->setPos(pos);

			// Returning:
			return obj;
		}

		// Cloning:
		virtual TVObject<T> *clone() const {
			// Creating a new instance using my position:
			return newInstance(_pos);
		}

		// Setting my internal state as the one of obj:
		virtual TVObject<T> *clone(const TVObject<T> *obj) {
			// Index:
			vector_dim_t i;

			// Base infos:
			_dim = obj->getDim();

			// Removing remitters:
			if (_rems) {
				// Iterating:
				for (i=0; i<_Nrems; i++) {
					// Deleting the remitter:
					delete _rems[i];
				}
			}

			// Must reallocate the remitters' vector?
			if (!_rems || _Nrems!=obj->getNrems()) {
				// Reallocating the remitters' vector:
				if (_rems) delete _rems;
				_Nrems = (unsigned char)obj->getNrems();
				_rems = new TVRemitter<T>*[_Nrems];
			}

			// Cloning:
			for (i=0; i<_Nrems; i++) {
				// Cloning:
				_rems[i] = obj->getRemitter(i)->newInstance(this);
			}

			// The position:
			setPosAggregated(obj->getPosAggregated());
			setPos(obj->getPos());

			// Other things:
			_Nvotes = obj->getNvotes();
			
			// Returning:
			return this;
		}

		// Removing an entire object:
		virtual ~TVObject() {
			// Deleting all the rems:
			if (_rems!=NULL && _Nrems>0) {
				// Iterating:
				for (vector_dim_t i=0; i<_Nrems; i++) {
					// Deleting:
					delete _rems[i];
				}
			}

			// Deleting the vector itself:
			if (_rems!=NULL) delete _rems;

			// The position:
			if (!_posAggregated && _pos!=NULL) delete _pos;
		}

		/***************************
		 * Structure manipulation: *
		 ***************************/

		// Setter for the position:
		TVObject<T> *setPos(T *pos) {
			// Checking the aggregation:
			if (_posAggregated) {
				// Saving the pointer (discarding the old one: this attribute is aggregated!):
				_pos = pos;
			}
			else {
				// Checking it it is a set or a reset:
				if (pos) {	// There is something to copy:
					// Allocating:
					if (!_pos) _pos = new T[_dim];

					// A copy must be made:
					memcpy(_pos,pos,_dim*sizeof(T));
				}
				else {	// Cleaning:
					// Unallocating:
					if (_pos) delete _pos;

					// None pointed:
					_pos = NULL;
				}
			}

			// Returning:
			return this;
		}

		// Getter for the position:
		T *getPos() const {
			// Returning:
			return _pos;
		}

		// Getter for the remitters by index:
		TVRemitter<T> *getRemitter(vector_dim_t i) const {
			// Checking:
			if (i<0 || i>=_Nrems || _rems==NULL) return NULL;

			// Returning:
			return _rems[i];
		}

		// Setter for the remitters array (_rems is a component array..
		// ..it must not be deallocated externally!):
		TVObject<T> *setRemitters(TVRemitter<T> **rems,vector_dim_t Nrems) {
			// Saving:
			_rems = rems;
			_Nrems = (unsigned char)Nrems;

			// Returning:
			return this;
		}

		// Getter for the dimensionality:
		vector_dim_t getDim() const {
			// Returning:
			return _dim;
		}

		// Getter for the number of remitters:
		vector_dim_t getNrems() const {
			// Returning:
			return _Nrems;
		}

		// Getter and setter for the pos aggregated:
		bool getPosAggregated() const {
			// Returning:
			return _posAggregated;
		}

		// WARNING: if aggregated is set to true when the position is not aggregated the
		//			point is cleaned!
		TVObject<T> *setPosAggregated(bool posAggregated) {
			// Comparing the actual state:
			if (posAggregated!=_posAggregated) {
				// Saving:
				_posAggregated = posAggregated;

				// Checking the case:
				if (posAggregated) {
					// Now it must be aggregated:
					if (_pos) delete _pos;
					_pos = NULL;
				}
				else {
					// Setting the position:
					T *pos = _pos;
					_pos = NULL;
					setPos(pos);
				}
			}

			// Returning:
			return this;
		}
		TVObject<T> *setPosAggregatedDirect(bool posAggregated) {
			// Setting:
			_posAggregated = posAggregated;

			// Returning:
			return this;
		}

		/*****************
		 * Voting tools: *
		 *****************/

		// Initialization of the voting process:
		virtual void init() {
			// Delegating:
			_TVO_DELEGATE_REMITTERS(_rems[i]->init());

			// No votes received (by now):
			_Nvotes = 0;
		}

		/*
		 *  Voting: here the actual object (this) is required to receive a vote (receiver)
		 * from the given object obj (emitter).
		 */
		virtual void vote(TVObject<T> *obj) {
			// The parameters:
			TVParameters<T> *params = TVParameters<T>::getInstance();

			// Checking the TVPARF_VOTE_T1_HALF flag:
			if (params->getFlag(TVPARF_VOTE_T1_HALF)) {
				// The checking must be done:
				// - Getting the T1 tensor from the emitter:
				TVRETensor1<T> *tns1 = dynamic_cast<TVRETensor1<T>*>(
					obj->getRemitter((vector_dim_t)params->getParameterInt(TVPARI_T1_INDEX)));
				T *T1, T1norm, T1normThresh;

				// The tensor must exist:
				if (tns1 && (T1=tns1->getTensor())) {
					// Getting the threshold for the norm:
					T1normThresh = params->getParameter(TVPAR_T1_HALF_NORM_THRESHOLD);

					// Computing the norm of T1:
					T1norm = T1normThresh<=EPS? 1: vec_norm_L2(T1,_dim);

					// The check is done only if the norm is sufficiently high:
					if (T1norm>=T1normThresh) {
						// Computing the dot product with a translated copy of my position:
						T *myPosTranslated = vec_create_clone<T>(NULL,getPos(),_dim),dotProd;
						vec_subtract(myPosTranslated,obj->getPos(),_dim);

						// Computing the dot product:
						dotProd = vec_dot(myPosTranslated,T1,_dim);

						// Freeing unused memory:
						vec_free(myPosTranslated);

						// If the dot product is negative no voting is done:
						if (dotProd<0) return;
					}
				}
			}

			// Delegating:
			_TVO_DELEGATE_REMITTERS(_rems[i]->vote(obj->getRemitter(i)));

			// Another vote received:
			_Nvotes++;
		}

		// The << operator do the same (but returns the new vote):
		TVObject<T> &operator <<(TVObject<T> *obj) {
			// Voting:
			vote(obj);

			// Returning:
			return *this;
		}

		// The >> operator returns the voter:
		TVObject<T> &operator >>(TVObject<T> *obj) {
			// Voting:
			obj->vote(this);

			// Returning:
			return *this;
		}

		// Updating:
		virtual void update() {
			// Delegating:
			_TVO_DELEGATE_REMITTERS(_rems[i]->update());
		}

		// Resetting:
		virtual void reset() {
			// Delegating:
			_TVO_DELEGATE_REMITTERS(_rems[i]->reset());
		}

		// Printing:
		virtual void fprint(FILE *fp=stderr) const {
			// Delegating:
			_TVO_DELEGATE_REMITTERS(fprintf(fp,"[REMITTER(%d)]\n",i); _rems[i]->fprint(fp));
		}

		// Returning the number of received votes:
		vector_dim_t getNvotes() const {
			// Returning:
			return _Nvotes;
		}
	};

} // namespace TVF

// Removing the local macros:
#undef _TVO_DELEGATE_REMITTERS

#endif //ndef _TVOBJECT_H_
