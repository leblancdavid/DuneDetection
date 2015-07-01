
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

#ifndef _TV_H_
#define _TV_H_

/*
 *  This file allows to include the whole TVF, single features
 * inclusion is also possible but unrecomended. The header file that
 * includes the base environment is TV_base.h, after the inclusion
 * of that header the single features can be manually selected.
 *  Consider that this lib is created to work with only a single
 * instance ative because the class TVParameters, used from all
 * the framework parts, is a singleton class. A mechanism that
 * allows to store tha actual parameters to work with another
 * set of params and to restore them in the future to restart
 * with the old workspace allows to work sequentially and
 * mutual-exlusively with different votings.
 */

// Including the base environment:
#include <TV_base.h>

// Including the extended tools:
#include <TV_EXT.h>

// Including the marching tools:
#include <TV_marching.h>

#endif //ndef _TV_H_
