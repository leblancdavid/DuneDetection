
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

#ifndef _TVPARAMETERS_H_
#define _TVPARAMETERS_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	*  This class represents the singleton parameters set
	* used during the whole voting process. This is the only
	* non-reentrant class of the TV framework (in the future this
	* can be substituted with a reentrant class that is non completely
	* singleton so that multiple parallel instances will be allowed).
	*/

	// The enumeration for the template parameters:
	typedef enum TVParameterID {
		TVPAR_RADIUS = 0,				// The radius of the range query:
		TVPAR_SCALE,					// The sigma value (scale) in the decay functions:
		TVPAR_T1_HALF_NORM_THRESHOLD,	// The T1 norm threshold used in the TVPARF_VOTE_T1_HALF feature:
		TVPAR_MARCH_THRESHOLD,			// The thresholding value for the marching procedure:
		TVPAR_EXTREMA_THRESHOLD,		// The thresholding value for the classification of extremal points:
		TVPAR_LAST						// This is a sentinel that allows to know the number of parameters.
	};

	// The enumeration for the int parameters:
	typedef enum TVParameterIntID {
		TVPARI_DIM = 0,		// The dimensionality of the space:
		TVPARI_REM_MASK,	// The usage's mask for the remitters (used and unused during the voting):
		TVPARI_REMS_NUM,	// The number of remitters:
		TVPARI_FLAGS,		// The TVParameterFlags:
		TVPARI_T1_INDEX,	// The index of the first order tensor if used:
		TVPARI_T2_INDEX,	// The index of the second order tensor if used:
		TVPARI_MARCH_FRONT_CORRECTIONS,	// The number of correction's steps for marching:
		TVPARI_LAST	// This is a sentinel that allows to know the number of parameters.
	};

	// The enumeration for the bit masks:
	typedef enum TVParameterMaskID {
		TVPARM_T1MASK = 0,	// Mask for the single kinds of first order base tensors:
		TVPARM_T2MASK = 0,	// Mask for the single kinds of second order base tensors:
		TVPARM_LAST	// This is a sentinel that allows to know the number of parameters.
	};

	// The set of flags:
	typedef enum TVParameterFlags {
		TVPARF_NORMALIZE		= 0x00000001,	// Normalization after T2 voting:
		TVPARF_T1MASK			= 0x00000002,	// Must the T1 base votes be masked?
		TVPARF_T2MASK			= 0x00000004,	// Must the T2 base votes be masked?
		TVPARF_CURVATURE_USE	= 0x00000008,	//  Uses the mean sectional curvature
												// to curve the decay function.
												// (to be used with TVDFOriginalCurvature)
		TVPARF_CURVATURE_COMP	= 0x00000010,	//  Computes the mean sectional curvature.
												// (to be used with TVRETensor2Curvature)
		TVPARF_CURVATURE_SALS	= 0x00000020,	//  Return as saliency the also curvature.
		TVPARF_VOTE_T1_HALF		= 0x00000040,	//  Require that the voting process executes
												// only in the half-space defined by the T1
												// tensor (index defined by TVPARI_T1_INDEX).
		TVPARF_T1MEAN			= 0x00000080,	//  Forces the T1 tensor to compute the mean
												// of votes instead of the simple sum.
		TVPARF_T2MEAN			= 0x00000100,	//  Forces the T2 tensor to compute the mean
												// of votes instead of the simple sum. (do not
												// works with TVPARF_NORMALIZE==true)
		TVPARF_T2INFORMATIVE_PRIOR
								= 0x00000200,	//  Defines the usage of the previous tensor
												// value as an informative prior (contrapposed
												// to the non-informative Gaussian prior).
		TVPARF_GRADIENT_APPROX_CENTRAL
								= 0x00000400,	// Central approximation of gradient.
		TVPARF_LAST				= 11
	};


	// The class:
	template <class T>
	class TVParameters {
	protected:
		/***************
		* Attributes: *
		***************/

		// The parameters vectors:
		T *_parv;
		long *_parintv;
		TVBitMask **_parbmv;

		// The static instance:
		static TVParameters<T> *_instance;

		// Max number of bits in the masks:
		static const int MASKS_BITS_MAX = 128;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// The default constructor:
		TVParameters() {
			// Allocating the parameters' vectors:
			_parv = vec_create_init<T>(NULL,(T)0,TVPAR_LAST);
			_parintv = vec_create_init<long>(NULL,0,TVPARI_LAST);
			_parbmv = new TVBitMask*[TVPARM_LAST];
			for (int i=0; i<TVPARM_LAST; i++) {
				_parbmv[i] = new TVBitMask(MASKS_BITS_MAX);
			}

			// Setting default values:
			setParameterInt(TVPARI_REM_MASK,0xFFFFFFFF);
            setParameterInt(TVPARI_FLAGS,0);
            setParameterInt(TVPARI_T1_INDEX,0);
            setParameterInt(TVPARI_T2_INDEX,1);
		}

		// Copy constructor:
		TVParameters(TVParameters<T> &pars) {
			// Allocating the parameters' vectors:
			_parv = vec_create_clone<T>(NULL,pars._parv,TVPAR_LAST);
			_parintv = vec_create_clone<long>(NULL,pars._parintv,TVPARI_LAST);
			_parbmv = new TVBitMask*[TVPARM_LAST];

			// Copy of the parameters:
			for (int i=0; i<TVPARM_LAST; i++) {
				_parbmv[i] = new TVBitMask(*pars._parbmv[i]);
			}
		}

		// Assignment operator:
		TVParameters<T> &operator =(TVParameters<T> &pars) {
			// Copy of the parameters:
			vec_create_clone<T>(_parv,pars._parv,TVPAR_LAST);
			vec_create_clone<long>(_parintv,pars._parintv,TVPARI_LAST);
			for (int i=0; i<TVPARM_LAST; i++) {
				*_parbmv[i] = *pars._parbmv[i];
			}
			return *this;
		}

		// Destructor:
		~TVParameters() {
			// Deallocating the parameters' vectors:
			vec_free(_parv);
			vec_free(_parintv);
			for (int i=0; i<TVPARM_LAST; i++) {
				delete _parbmv[i];
			}
			delete _parbmv;
		}

		// Getting an instance:
		static TVParameters<T> *getInstance() {
			// Checking:
			if (_instance==NULL) {
				// Creating a new instance:
				_instance = new TVParameters<T>;
			}

			// Returning:
			return _instance;
		}

		// Setting a new instance (old parameters are deleted):
		static TVParameters<T> *setInstance(TVParameters<T> *pars) {
			// Removing the old parameters:
			if (_instance) delete _instance;

			// Saving the instance:
			_instance = pars;

			// Returning:
			return _instance;
		}

		// Getter for a parameter:
		T getParameter(TVParameterID ID) const {
			// Returning the corresponding parameter:
			return _parv[ID];
		}

		// Setter for a parameter:
		TVParameters<T> *setParameter(TVParameterID ID,T value) {
			// Setting the corresponding parameter:
			_parv[ID] = value;

			// Returning:
			return this;
		}

		// Getter for an integer parameter:
		long getParameterInt(TVParameterIntID ID) const {
			// Returning the corresponding parameter:
			return _parintv[ID];
		}

		// Setter for an integer parameter:
		TVParameters<T> *setParameterInt(TVParameterIntID ID,long value) {
			// Setting the corresponding parameter:
			_parintv[ID] = value;

			// Returning:
			return this;
		}

		// Getter for a mask parameter:
		TVBitMask *getParameterMask(TVParameterMaskID ID) const {
			// Returning the corresponding parameter:
			return _parbmv[ID];
		}

		// Getter for one flag:
		bool getFlag(TVParameterFlags flag) const {
			// Getting the flag value and returning:
			return (_parintv[TVPARI_FLAGS] & flag)!=0;
		}

		// Setter for one flag:
		TVParameters<T> *setFlag(TVParameterFlags flag,bool value) {
			// Setting the flag value:
			if (value) {
				// Setting the flag to one:
				_parintv[TVPARI_FLAGS] |= flag;
			}
			else {
				// Setting the flag to zero:
				_parintv[TVPARI_FLAGS] &= ~flag;
			}

			// Returning:
			return this;
		}
	};

	// Static attributes:
	template <class T>
	TVParameters<T> *TVParameters<T>::_instance = NULL;

} // namespace TVF

#endif //ndef _TVPARAMETERS_H_
