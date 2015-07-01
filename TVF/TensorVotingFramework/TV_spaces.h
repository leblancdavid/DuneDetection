
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

#ifndef _TV_SPACES_H_
#define _TV_SPACES_H_

/*
 *  This namespace contains the implementation of various types of
 * space data structures.
 */

// The simple vector space:
#include <Spaces/TVSpVector.h>
#include <Spaces/TVSpImg.h>
#include <Spaces/TVSpVoxHash.h>
#include <Spaces/TVSpKdTree.h>

// The space filters:
#include <TV_space_filters.h>

#endif //ndef _TV_SPACES_H_
