
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

#ifndef _TV_EXT_CURVATURE_H_
#define _TV_EXT_CURVATURE_H_

// The decay functions:
#include <Curvature/TVDFCurvatureBase.h>
#include <Curvature/TVDFCurvature.h>

// The remitter:
#include <Curvature/TVRETensor2Curvature.h>

// The curvature evaluation/usage recipe:
#include <Curvature/TVRecCurvature.h>


// Predefined types templates' lists:
using namespace TVF;
using namespace TVF::Extensions::Curvature;

//  Creating a set of types lists with T1 and T2 tensorial remitters 
// and compressed decays with curvature:
template <class T>
struct TVRETypesT1T2CompCurvature : 
	public TVRETypesT1T2<T,TVRETensor2Curvature<T>,TVDFCurvature<T,TVDFComp<T> > > {};

//  Creating a set of types lists with T1 and T2 tensorial remitters 
// and original decays with curvature:
template <class T>
struct TVRETypesT1T2OriginalCurvature : 
	public TVRETypesT1T2<T,TVRETensor2Curvature<T>,TVDFCurvature<T,TVDFOriginal<T> > > {};

#endif //ndef _TV_EXT_CURVATURE_H_
