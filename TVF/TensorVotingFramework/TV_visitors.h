
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

#ifndef _TV_VISITORS_H_
#define _TV_VISITORS_H_

/*
 *  This namespace contains the visitors that allows to manipulate the
 * informations embedded in the objects. The terminals are a special kind
 * of visitors that allow to generate an output over a specific terminal like
 * an image, a file or other kind of data saving or figure drawing terminals. 
 */

// Interfaces:
#include <Visitors/ITVTerminal.h>

// The base visitors:
#include <Visitors/TVVRPlot.h>
#include <Visitors/TVVORemitters.h>
#include <Visitors/TVVORPlot.h>

// Concrete visitors:
#include <Visitors/TVTermWriteMFile.h>
#include <Visitors/TVTermWriteMemory.h>
#include <Visitors/TVTermReadMemory.h>
#include <Visitors/TVVOSaliency.h>
#include <Visitors/TVVOPositions.h>

#endif //ndef _TV_VISITORS_H_
