
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

#ifndef _ITVVISITREMITTER_H_
#define _ITVVISITREMITTER_H_

// All this in the plain TVF namespace:
namespace TVF {

	/*
	*  This class represent a general visitor for the remitter hierarchy.
	*/
	template <class T>
	class ITVVisitRemitter {
	public:
		// Destructor:
		virtual ~ITVVisitRemitter() {}

		// Initialization:
		virtual void init(TVObject<T> *obj) {}

		// The visit method:
		virtual void visit(TVRemitter<T> *rem)=0;

		// Finalization:
		virtual void finalize() {}
	};

} // namespace TVF

#endif //ndef _ITVVISITREMITTER_H_
