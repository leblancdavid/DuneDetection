
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

#ifndef _TVRETENSOR1_H_
#define _TVRETENSOR1_H_

/*
 *  This class allows to manage first order tensorial voting
 * to capture the directionality of votes.
 */

// All this in the TVF namespace:
namespace TVF {

	// Namespaces used:
	using namespace std;
	using namespace TVF::Math;

	/*
	 *  A first order tensor.
	 */
	template <class T>
	class TVRETensor1 : public TVRemitter<T> {
	protected:
		/***************
		 * Attributes: *
		 ***************/

		// My first order tensor:
		T *_tns;

		// My decay function:
		static ITVDecayFun<T> *_decf;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// Default constructor:
		TVRETensor1(TVObject<T> *obj) : TVRemitter<T>(obj), _tns(NULL) {
			// Allocating the first order tensor:
			reset();
		}

		// Destructor:
		~TVRETensor1() {
			// Deleting the tensor:
			vec_free(_tns);
		}

		/***************************
		 * Structure manipulation: *
		 ***************************/

		// Generating a new instance:
		TVRemitter<T> *newInstance(TVObject<T> *obj) const {
			// Using the constructor:
			TVRETensor1<T> *re = new TVRETensor1<T>(obj);

			// Cloning the actual initialization:
			vector_dim_t dim = getDim();
			vec_create_clone(re->_tns,_tns,dim);

			// Returning:
			return re;
		}

		/*************************************
		 * Managment of the decay functions: *
		 *************************************/

		// Setting the decay function:
		static void _setDecay(ITVDecayFun<T> *func) {
			// Removing the old one:
			if (_decf!=NULL) delete _decf;
			_decf = func;
		};

		// Getting the decay function:
		static ITVDecayFun<T> *_getDecay() {
			// Returning:
			return _decf;
		};

		// Setting the decay function:
		virtual void setDecay(ITVDecayFun<T> *func) {
			// Using the correct static function:
			_setDecay(func);
		};

		// Getting the decay function:
		virtual ITVDecayFun<T> *getDecay() const {
			// Using the correct static function:
			return _getDecay();
		};

		/*****************
		 * Voting tools: *
		 *****************/

		// Resetting the T1 tensor:
		virtual void reset() {
			// Zeroing the tensor:
			vector_dim_t dim = getDim();
			_tns = vec_create_init<T>(_tns,0,dim);
		}

		// Initialization of the voting process:
		virtual void init() {
			// The same as clean:
			reset();
		}

		/*
		 *  Voting:
		 * EMITTER:		rem
		 * RECEIVER:	this
		 */
		virtual void vote(TVRemitter<T> *rem) {
			// Parameters:
			TVParameters<T> *pars = TVParameters<T>::getInstance();

			// The dimensionality:
			vector_dim_t dim = getDim();

			// Getting the T2 tensor:
			TVRETensor2<T> *T2 = dynamic_cast<TVRETensor2<T>*>(
				rem->getObject()->getRemitter(
					(vector_dim_t)pars->getParameterInt(TVPARI_T2_INDEX)));

			// Getting the decoded tensor:
			tensor_decoded_t<T> *tnsd = T2? T2->getDecodedTensor(): NULL;

			// The bitmask:
			ITVBitMask *bm = pars->getFlag(TVPARF_T1MASK)?
				pars->getParameterMask(TVPARM_T1MASK): NULL;

			// Computing the result:
			T *res = NULL;
			res = vec_create_vote<T>(getObject()->getPos(),rem->getObject()->getPos(),
				tnsd,NULL,_decf,bm,dim);

			// Accumulating it:
			vec_accumulate(_tns,res,dim);

			// Cleaning:
			vec_free(res);
		}

		// Updating:
		virtual void update() {
			// The paramenters:
			TVParameters<T> *params = TVParameters<T>::getInstance();

			// The final vote is a mean:
			if (params->getFlag(TVPARF_T1MEAN)) {
				// Scaling with the number of votes received:
				vector_dim_t N = getObject()->getNvotes();
				if (N>0) {
					// Scaling:
					vec_scale(_tns,(T)1.0/N,getDim());
				}
			}
		}

		// Computing the saliencies:
		virtual T *computeSals(T *buff=NULL) const {
			// The array to be returned:
			if (buff==NULL) buff = new T[1];

			// The saliency here is the norm:
			buff[0] = vec_norm_L2<T>(getTensor(),getDim());

			// Returning:
			return buff;
		}

		// Sayng the number of saliency values generated:
		virtual vector_dim_t getSaliencyNum() const {
			// A fixed value:
			return 1;
		}

		// Printing:
		virtual void fprint(FILE *fp=stderr) const {
			// Printing the first order tensor:
			vector_dim_t dim = getDim();
			vec_fprint<T>(fp,_tns,dim);
		}

		/*****************
		 * Custom tools: *
		 *****************/

		// Getter for one of the T1 tensors:
		T *getTensor() const {
			// Returning:
			vector_dim_t dim = getDim();
			return _tns;
		}

		// Setting one of the T1 tensors (as a copy):
		void setTensor(T *T1) {
			// Copying on the current tensor:
			_tns = vec_create_clone(_tns,T1,getDim());
		}
	};

	// My decay function:
	template <class T>
	ITVDecayFun<T> *TVRETensor1<T>::_decf = NULL;

} // namespace TVF

#endif //ndef _TVRETENSOR1_H_
