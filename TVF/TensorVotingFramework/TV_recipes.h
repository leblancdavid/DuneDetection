
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

#ifndef _TV_RECIPES_H_
#define _TV_RECIPES_H_

/*
 *  Here all the classes implementing the ITVRecipe interface,
 * are collected: the recipes are objects that can manage a
 * voting algorithm or a single voting pass setting the parameters
 * and then executing the voting in one or another manner.
 */

// Recipe's interface:
#include <Recipes/ITVRecipe.h>

// The plain single voting recipe:
#include <Recipes/TVRecPlain.h>

// The composite recipe:
#include <Recipes/TVRecComp.h>

#endif //ndef _TV_RECIPES_H_
