/*
 *  tparser.h
 *
 *  Copyright (c) April 2007 Daniel M. German <dmgerman at uvic doooot ca>
 *  Copyright (c) 2012 Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <assert.h>
#include <stddef.h>

#include "tparsergetters.h"


int panoScriptGetInputImagesCount(pt_script* script)
{
    assert(script != NULL);
    return script->iInputImagesCount;
}

float panoScriptGetImageCoefA(pt_script* script, int i)
{
    assert(script != NULL);
    assert(i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[0];
}

float panoScriptGetImageCoefB(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[1];
}

float panoScriptGetImageCoefC(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[2];
}

float panoScriptGetImageCoefD(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[3];
}

float panoScriptGetImageCoefE(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[4];
}

float panoScriptGetImageSheerX(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[5];
}

float panoScriptGetImageSheerY(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].geometryCoef[6];
}

int panoScriptGetImageProjection(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].projection;
}

float panoScriptGetImageHFOV(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].fHorFOV;
}

float panoScriptGetImagePitch(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].pitch;
}

float panoScriptGetImageYaw(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].yaw;
}

float panoScriptGetImageRoll(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].roll;
}

char* panoScriptGetName(pt_script* script, int i)
{
    assert(script != NULL && i >= 0 && i < script->iInputImagesCount);
    return script->inputImageSpec[i].name;
}





int panoScriptGetPanoProjection(pt_script* script)
{
    assert(script != NULL);
    return script->pano.projection;
}

int panoScriptGetPanoWidth(pt_script* script)
{
    assert(script != NULL);
    return script->pano.width;
}

int panoScriptGetPanoHeight(pt_script* script)
{
    assert(script != NULL);
    return script->pano.height;
}

float panoScriptGetPanoHFOV(pt_script* script)
{
    assert(script != NULL);
    return script->pano.fHorFOV;
}

float panoScriptGetPanoParmsCount(pt_script* script)
{
    assert(script != NULL);
    return script->pano.projectionParmsCount;
}

float panoScriptGetPanoParm(pt_script* script, int index)
{
    assert(script != NULL && index < script->pano.projectionParmsCount);
    return script->pano.projectionParms[index];
}

char* panoScriptGetPanoOutputFormat(pt_script* script)
{
    assert(script != NULL);
    return script->pano.outputFormat;
}
