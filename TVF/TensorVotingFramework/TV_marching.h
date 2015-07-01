
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

#ifndef _TV_MARCHING_H_
#define _TV_MARCHING_H_

/*
 *  This namespace contains the marching tools that allows to reconstruct
 * manifolds inferred from the tensors after the voting process.
 */

// Maximizers:
#include <Marching/ITVSaliencyMaximizer.h>
#include <Marching/TVSMDichotomicGradient.h>

// Correctors:
#include <Marching/ITVCorrector.h>
#include <Marching/TVCorrMaximizer.h>

// The expanding objects:
#include <Marching/TVObjectExpand.h>
#include <Marching/TVObjectExpandCurve.h>
#include <Marching/TVObjectExpandSurface.h>

// The marching classes:
#include <Marching/TVMarch.h>
#include <Marching/TVMarchSequential.h>


#endif //ndef _TV_MARCHING_H_
