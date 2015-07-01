
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

#ifndef _TVBITMASK_H_
#define _TVBITMASK_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	 *  This class represents a mask of bits that can be set or
	 * reset. The mask is of fixed size.
	 */

	// Usings:
	using namespace TVF::Math;
	using namespace TVF::DataStructures;

	class TVBitMask : public ITVBitMask {
	protected:
		/***************
		* Attributes: *
		***************/

		// The pointer to the mask:
		vector_ind_t *_mask;

		// The number of elements:
		vector_ind_t _numel;

	public:
		/********************************
		 * Constructors and destructor: *
		 ********************************/

		// Creating an empty mask for (at least) a given number of bits:
		TVBitMask(vector_ind_t numBit) {
			// Computing the number of elements:
			_numel = (vector_ind_t)ceilf((float)numBit/(8*sizeof(vector_ind_t)));

			// Allocating:
			_mask = new vector_ind_t[_numel];
		}

		// Copy constructor:
		TVBitMask(TVBitMask &bm) {
			// Copy of data:
			_numel = bm._numel;
			_mask = new vector_ind_t[_numel];
			memcpy(_mask,bm._mask,_numel*sizeof(vector_ind_t));
		}

		// Assignment:
		TVBitMask &operator =(TVBitMask &bm) {
			// Deleting the actual mask:
			delete _mask;

			// Copy:
			_numel = bm._numel;
			_mask = new vector_ind_t[_numel];
			memcpy(_mask,bm._mask,_numel*sizeof(vector_ind_t));

			// Returning:
			return *this;
		}

		// Destructor:
		virtual ~TVBitMask() {
			// Deallocating the mask:
			delete _mask;
		}

		/**************************
		 * Accessors to the bits: *
		 **************************/

		// Getting the value of a bit:
		virtual bool getBit(vector_ind_t i) const {
			// Indexes:
			vector_ind_t j,k;

			// The element index:
			j = i/(8*sizeof(vector_ind_t));

			// Check:
			if (j>=_numel) {
				// Very bad!
				return false;
			}

			// The bit position:
			k = i%(8*sizeof(vector_ind_t));

			// Returning:
			return ((_mask[j]>>k)&0x01)!=0;
		}

		// Getting the value of a bit:
		virtual ITVBitMask *setBit(vector_ind_t i,bool value) {
			// Indexes:
			vector_ind_t j,k;

			// The element index:
			j = i/(8*sizeof(vector_ind_t));

			// Check:
			if (j>=_numel) {
				// Very bad!
				return this;
			}

			// The bit position:
			k = i%(8*sizeof(vector_ind_t));

			// Setting:
			if (value) {
				// Setting one:
				_mask[j] |= 1<<k;
			}
			else {
				// Setting zero:
				_mask[j] &= ~(1<<k);
			}

			// Returning:
			return this;
		}

		// Setting all the bits:
		virtual ITVBitMask *setAll(bool value) {
			// The single byte value:
			unsigned char c;

			// Checking if it is a set or reset:
			c = value? 0xFF: 0x00;

			// Setting:
			memset(_mask,c,_numel*sizeof(vector_ind_t));

			// Returning:
			return this;
		}

		/***************************
		 * Accessors to the infos: *
		 ***************************/

		// Getter for the max number of bits:
		virtual vector_ind_t getSize() const {
			// Returning:
			return _numel*8*sizeof(vector_ind_t);
		}
	};

} // namespace TVF

#endif //ndef _TVBITMASK_H_
