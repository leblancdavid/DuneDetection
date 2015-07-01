
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

#ifndef _TV_BASE_H_
#define _TV_BASE_H_

/*
 *  In this file are included and defined the tools related to
 * the base TV environment, without extensions. The invironment
 * includes a set of interfaes and abstract classes (plus some
 * implementation class) that must be extended to become useful
 * partes of the final application.
 */

// External library:
#include <iostream>
#include <vector>
#include <hash_map>

// The template tools:
#include <TV_template_tools.h>

// The math stuff:
#include <TV_math.h>

// The classes, abstract classes and interfaces that define the environment:
#include <TVBitMask.h>
#include <TVParameters.h>
#include <TVRemitter.h>
#include <ITVIterator.h>
#include <TVObject.h>
#include <TVSpace.h>
#include <ITVVisitObject.h>
#include <ITVVisitRemitter.h>
#include <ITVVotingSystem.h>

// Including the spaces:
#include <TV_spaces.h>

// Additional data structures:
#include <TV_data_structures.h>

// Including the remitters:
#include <TV_remitters.h>

// Including the process tools:
#include <TV_process_tools.h>

// Including the visitors:
#include <TV_visitors.h>

// Cleaning tools:
#include <TV_cleaning.h>


#endif //ndef _TV_BASE_H_
