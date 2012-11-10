/*============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Description : Hugin parser API
 *
 * Copyright (C) 2007 Daniel M German <dmgerman at uvic doooot ca>
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of 
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================ */

#ifndef TPARSERGETTERS_H
#define TPARSERGETTERS_H

#include "tparser.h"

int     panoScriptGetInputImagesCount(pt_script* script);
float   panoScriptGetImageCoefA(pt_script* script, int i);
float   panoScriptGetImageCoefB(pt_script* script, int i);
float   panoScriptGetImageCoefC(pt_script* script, int i);
float   panoScriptGetImageCoefD(pt_script* script, int i);
float   panoScriptGetImageCoefE(pt_script* script, int i);
float   panoScriptGetImageSheerX(pt_script* script, int i);
float   panoScriptGetImageSheerY(pt_script* script, int i);
int     panoScriptGetImageProjection(pt_script* script, int i);
float   panoScriptGetImageHFOV(pt_script* script, int i);
float   panoScriptGetImagePitch(pt_script* script, int i);
float   panoScriptGetImageYaw(pt_script* script, int i);
float   panoScriptGetImageRoll(pt_script* script, int i);
char*   panoScriptGetName(pt_script* script, int i);

int     panoScriptGetPanoProjection(pt_script* script);
int     panoScriptGetPanoWidth(pt_script* script);
int     panoScriptGetPanoHeight(pt_script* script);
float   panoScriptGetPanoHFOV(pt_script* script);
float   panoScriptGetPanoParmsCount(pt_script* script);
float   panoScriptGetPanoParm(pt_script* script, int index);
char*   panoScriptGetPanoOutputFormat(pt_script* script);

#endif
