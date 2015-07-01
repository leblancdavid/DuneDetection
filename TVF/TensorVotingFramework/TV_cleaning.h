
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

#ifndef _CLEANING_H_
#define _CLEANING_H_

// The namespace:
namespace TVF {

	// Cleaning:
	template <class T>
	void TVF_clean() {
		// Removing the static variables:
		// - Parameters:
		TVParameters<T>::setInstance(NULL);
		// - Tensors decays:
		TVRETensor1<T>::_setDecay(NULL);
		TVRETensor2<T>::_setDecay(NULL);

		// Cleaning the math lib buffers:
		mathFwk_free_static_buffs<T>();
	}

} // namespace TVF

#endif //ndef _CLEANING_H_
