
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

#ifndef _TVRETENSOR2_H_
#define _TVRETENSOR2_H_

/*
 *  This class allows to manage first order tensorial voting
 * to capture the directionality of votes.
 */

// All this in the TVF namespace:
namespace TVF {

	// Using the math namespace:
	using namespace TVF::Math;

	/*
	 *  A second order tensor.
	 */
	template <class T>
	class TVRETensor2 : public TVRemitter<T> {
	protected:
		/***************
		 * Attributes: *
		 ***************/

		// The tensor:
		tensor_t<T> _tns;

		// The decoded version of the tensor:
		tensor_decoded_t<T> *_tnsd;

		// My decay function:
		static ITVDecayFun<T> *_decf;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// Default constructor:
		TVRETensor2(TVObject<T> *obj) : TVRemitter<T>(obj), _tns(NULL), _tnsd(NULL) {
			// Cleaning:
			reset();
		}

		// Destructor:
		~TVRETensor2() {
			// Deleting the tensors:
			cleanTnsd();
			_tns = tns_free(_tns);
		}

		/***************************
		 * Structure manipulation: *
		 ***************************/

		// Generating a new instance:
		TVRemitter<T> *newInstance(TVObject<T> *obj) const {
			// Using the constructor:
			TVRETensor2<T> *re = new TVRETensor2<T>(obj);

			// Cloning the actual initialization:
			memcpy(re->_tns,_tns,TNS_DOF_NUM(getDim())*sizeof(T));

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
		void setDecay(ITVDecayFun<T> *func) {
			// Using the correct static function:
			_setDecay(func);
		};

		// Getting the decay function:
		ITVDecayFun<T> *getDecay() const {
			// Using the correct static function:
			return _getDecay();
		};

		/*****************
		 * Voting tools: *
		 *****************/

		// Initialization of the voting process:
		virtual void init() {
			// Getting the parameters:
			TVParameters<T> *pars = TVParameters<T>::getInstance();

			// The dimensionality:
			vector_dim_t dim = getDim();

			// Decoding the tensor:
			cleanTnsd();
			_tnsd = tns_decode(_tns,dim);

			// Must the prior be non-informative?
			if (!pars->getFlag(TVPARF_T2INFORMATIVE_PRIOR)) {
				// Zeroing the old tensor:
				tns_create_empty(_tns,dim);
			}
		}

		/*
			*  Voting:
			* EMITTER:		rem
			* RECEIVER:	this
			*/
		virtual void vote(TVRemitter<T> *rem) {
			// Getting the parameters:
			TVParameters<T> *pars = TVParameters<T>::getInstance();

			// Casting the remitter:
			TVRETensor2<T> *r = dynamic_cast<TVRETensor2<T>*>(rem);
			if (r==NULL) {
				// Wrong type given:
				cerr << "A wrong remitter type has voted on a TVRETensor2!" << endl;
				return;
			}

			// The dimensionality:
			vector_dim_t dim = getDim();

			// The resulting tensor:
			tensor_t<T> res = tns_create<T>(dim);

			// The bitmask:
			ITVBitMask *bm = pars->getFlag(TVPARF_T2MASK)?
				pars->getParameterMask(TVPARM_T2MASK): NULL;

			// Voting:
			tns_create_vote<T>(	getObject()->getPos(),
								rem->getObject()->getPos(),
								r->getDecodedTensor(),
								res,_decf,bm,dim);

			// Accumulating:
			tns_accumulate(_tns,res,dim);

			// Freeing memory:
			tns_free(res);
		}

		// Computing the saliencies:
		virtual T *computeSals(T *buff=NULL) const {
			// The dimensionality:
			vector_dim_t dim = getDim();

			// Checking the case:
			if (_tnsd) {
				// Computing the saliency:
				buff = tnsd_compute_saliency(_tnsd,buff,dim);
			}
			else {
				// Decoding:
				tensor_decoded_t<T> *tnsd = tns_decode(_tns,dim,false);

				// Computing the saliency:
				buff = tnsd_compute_saliency(tnsd,buff,dim);

				// Freeing memory:
				tnsd_free(tnsd);
			}

			// Returning:
			return buff;
		}

		// Sayng the number of saliency values generated:
		virtual vector_dim_t getSaliencyNum() const {
			// Same as dim:
			return getDim();
		}

		// Updating:
		virtual void update() {
			// The paramenters:
			TVParameters<T> *params = TVParameters<T>::getInstance();

			// The update corresponds to the clean operation:
			cleanTnsd();

			// Check if the resulting tensor must be normalized:
			if (params->getFlag(TVPARF_NORMALIZE)) {
				// Normalization:
				tns_normalize<T>(_tns,getDim(),tns_norm_L2<T>);
			}
			else if (params->getFlag(TVPARF_T2MEAN)) {
				// Scaling with the number of votes received:
				vector_dim_t N = getObject()->getNvotes();
				if (N>0) {
					// Scaling:
					tns_scale(_tns,(T)1.0/N,getDim());
				}
			}
		}

		// Cleaning the decoded tensor:
		void cleanTnsd() {
			// Deallocating the decoded tensor:
			tnsd_free(_tnsd);
			_tnsd = NULL;
		}

		// Resetting:
		virtual void reset() {
			// Deallocating the decoded tensor:
			cleanTnsd();

			// Setting the encoded tensor to ball:
			_tns = tns_create_ball<T>(_tns,_obj->getDim());
		}

		// Printing:
		virtual void fprint(FILE *fp=stderr) const {
			// Printing the first order tensor:
			tns_fprint<T>(fp,_tns,getDim());
		}

		/*****************
		 * Custom tools: *
		 *****************/

		// Getter for the tensor:
		tensor_t<T> getTensor() const {
			// Returning the encoded tensor:
			return _tns;
		}

		// Setting the T1 tensor (as a copy):
		void setTensor(tensor_t<T> T2) {
			// Copying on the current tensor:
			_tns.ptr = vec_create_clone(_tns.ptr,T2.ptr,TNS_DOF_NUM(getDim()));

			// Updating the decoded tensor:
			if (_tnsd) {
				// Cleaning the old decoded tensor:
				tnsd_free(_tnsd);

				// Decoding it again:
				_tnsd = tns_decode(_tns,getDim());
			}
		}

		// Getter for the decoded tensor (lazy evaluated):
		tensor_decoded_t<T> *getDecodedTensor() {
			// Decoding:
			if (_tnsd==NULL) {
				// Decoding:
				_tnsd = tns_decode(_tns,getDim());
			}

			// Returning the decoded tensor:
			return _tnsd;
		}

		// Inverting the tensor from attractive to repulsive:
		void invert() {
			//  Changing simply the sign of the encoded tensor
			// (and the decoded too if it is present):
			tns_scale<T>(_tns,-1,getDim());

			// Checking the decoded version:
			if (_tnsd) {
				// Inverting the sign of the eigenvalues:
				vec_scale<T>(_tnsd->eigenValues,-1,getDim());
			}
		}
	};

	// My decay function:
	template <class T>
	ITVDecayFun<T> *TVRETensor2<T>::_decf = NULL;

} // namespace TVF

#endif //ndef _TVRETENSOR2_H_
