/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-04
 * Description : a plugin to create panorama by fusion of several images.
 *               This parser is based on pto file format described here:
 *               http://hugin.sourceforge.net/docs/nona/nona.txt, and
 *               on pto files produced by Hugin's tools.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IMAGELINE_DEF_H
#define IMAGELINE_DEF_H

#include "imageline.h"

#include "kdebug.h"


namespace KIPIPanoramaPlugin { namespace PtoParser
{

typedef enum {
    HEIGHT, WIDTH, PROJECTION, FOV, YAW, PITCH, ROLL,
    BARREL_A, BARREL_B, BARREL_C, CENTER_X, CENTER_Y, SHEAR_X, SHEAR_Y,
    EXPOSURE, WB_RED, WB_BLUE,
    V_MODE, V_I, V_J, V_K, V_L, VOFFSET_X, VOFFSET_Y, VFLATFIELD,
    EMOR_A, EMOR_B, EMOR_C, EMOR_D, EMOR_E, MOSAIC_X, MOSAIC_Y, MOSAIC_Z,
    CROP, STACK, FILENAME
} ImageParameter;

struct imageParameterInt_ : qi::symbols<char, ImageParameter>
{
    imageParameterInt_()
    {
        add
        ("w",   WIDTH)
        ("h",   HEIGHT)
        ("f",   PROJECTION)
        ("Vm",  V_MODE)
        ("TrX", MOSAIC_X)
        ("TrY", MOSAIC_Y)
        ("TrZ", MOSAIC_Z)
        ("j",   STACK);
    }
} imageParameterInt;

void setParameterFromInt(ImageParameter p, int i, PTOType::Image& image)
{
    switch (p)
    {
        case WIDTH:
            image.size.setWidth(i);
            break;
        case HEIGHT:
            image.size.setHeight(i);
            break;
        case PROJECTION:
            image.lensProjection = PTOType::Image::LensProjection(i);
            break;
        case V_MODE:
            image.vignettingMode = PTOType::Image::VignettingMode(i);
            break;
        case MOSAIC_X:
            image.mosaicModeOffsetX = i;
            break;
        case MOSAIC_Y:
            image.mosaicModeOffsetY = i;
            break;
        case MOSAIC_Z:
            image.mosaicModeOffsetZ = i;
            break;
        case STACK:
            image.stackNumber = i;
            break;
        default:
            kDebug() << "Wrong parameter type!!";
    }
}

struct imageParameterDouble_ : qi::symbols<char, ImageParameter>
{
    imageParameterDouble_()
    {
        add
        ("y",   YAW)
        ("p",   PITCH)
        ("r",   ROLL)
        ("Eev", EXPOSURE)
        ("Er",  WB_RED)
        ("Eb",  WB_BLUE);
    }
} imageParameterDouble;

void setParameterFromDouble(ImageParameter p, double d, PTOType::Image& image)
{
    switch (p)
    {
        case YAW:
            image.yaw = d;
            break;
        case PITCH:
            image.pitch = d;
            break;
        case ROLL:
            image.roll = d;
            break;
        case EXPOSURE:
            image.exposure = d;
            break;
        case WB_RED:
            image.whiteBalanceRed = d;
            break;
        case WB_BLUE:
            image.whiteBalanceBlue = d;
            break;
        default:
            kDebug() << "Wrong parameter type!!";
    }
}

struct imageParameterLPInt_ : qi::symbols<char, ImageParameter>
{
    imageParameterLPInt_()
    {
        add
        ("d",   CENTER_X)
        ("e",   CENTER_Y)
        ("g",   SHEAR_X)
        ("t",   SHEAR_Y)
        ("Vx",  VOFFSET_X)
        ("Vy",  VOFFSET_Y);
    }
} imageParameterLPInt;

void setParameterFromLPInt(ImageParameter p, PTOType::Image::LensParameter<int> lp, PTOType::Image& image)
{
    switch (p)
    {
        case CENTER_X:
            image.lensCenterOffsetX = lp;
            break;
        case CENTER_Y:
            image.lensCenterOffsetY = lp;
            break;
        case SHEAR_X:
            image.lensShearX = lp;
            break;
        case SHEAR_Y:
            image.lensShearY = lp;
            break;
        case VOFFSET_X:
            image.vignettingOffsetX = lp;
            break;
        case VOFFSET_Y:
            image.vignettingOffsetY = lp;
            break;
        default:
            kDebug() << "Wrong parameter type!!";
    }
}

struct imageParameterLPDouble_ : qi::symbols<char, ImageParameter>
{
    imageParameterLPDouble_()
    {
        add
        ("v",   FOV)
        ("a",   BARREL_A)
        ("b",   BARREL_B)
        ("c",   BARREL_C)
        ("Va",  V_I)
        ("Vb",  V_J)
        ("Vc",  V_K)
        ("Vd",  V_L)
        ("Ra",  EMOR_A)
        ("Rb",  EMOR_B)
        ("Rc",  EMOR_C)
        ("Rd",  EMOR_D)
        ("Re",  EMOR_E);
    }
} imageParameterLPDouble;

void setParameterFromLPDouble(ImageParameter p, PTOType::Image::LensParameter<double> lp, PTOType::Image& image)
{
    switch (p)
    {
        case FOV:
            image.fieldOfView = lp;
            break;
        case BARREL_A:
            image.lensBarrelCoefficientA = lp;
            break;
        case BARREL_B:
            image.lensBarrelCoefficientB = lp;
            break;
        case BARREL_C:
            image.lensBarrelCoefficientC = lp;
            break;
        case V_I:
            image.vignettingCorrectionI = lp;
            break;
        case V_J:
            image.vignettingCorrectionJ = lp;
            break;
        case V_K:
            image.vignettingCorrectionK = lp;
            break;
        case V_L:
            image.vignettingCorrectionL = lp;
            break;
        case EMOR_A:
            image.photometricEMoRA = lp;
            break;
        case EMOR_B:
            image.photometricEMoRB = lp;
            break;
        case EMOR_C:
            image.photometricEMoRC = lp;
            break;
        case EMOR_D:
            image.photometricEMoRD = lp;
            break;
        case EMOR_E:
            image.photometricEMoRE = lp;
            break;
        default:
            kDebug() << "Wrong parameter type!!";
    }
}

template <typename Iterator>
imageLineGrammar<Iterator>::imageLineGrammar() : imageLineGrammar::base_type(line)
{
    using qi::int_;
    using qi::double_;
    using qi::_val;
    using qi::_1;
    using qi::lexeme;
    using phoenix::bind;
    using phoenix::construct;
    using phoenix::ref;
    typedef PTOType::Image Image;

    // ------------------------- Image line parsing -------------------------

    lensParameterInt =
          int_                          [bind(&Image::LensParameter<int>::value, _val) = _1]
        | '=' >> int_                   [bind(&Image::LensParameter<int>::referenceId, _val) = _1];

    lensParameterDouble =
          int_                          [bind(&Image::LensParameter<double>::value, _val) = _1]
        | '=' >> double_                [bind(&Image::LensParameter<double>::referenceId, _val) = _1];

    ImageParameter p;

    line = *(
          imageParameterInt             [ref(p) = _1]
            >> int_                     [bind(&setParameterFromInt, ref(p), _1, _val)]
        | imageParameterDouble          [ref(p) = _1]
            >> double_                  [bind(&setParameterFromDouble, ref(p), _1, _val)]
        | imageParameterLPInt           [ref(p) = _1]
            >> lensParameterInt         [bind(&setParameterFromLPInt, ref(p), _1, _val)]
        | imageParameterLPDouble        [ref(p) = _1]
            >> lensParameterDouble      [bind(&setParameterFromLPDouble, ref(p), _1, _val)]
        | "Vf\""
            >> lexeme[string            [bind(&Image::vignettingFlatfieldImageName, _val) = _1]]
            >> '"'
        | 'S' >> rectangle              [bind(&Image::crop, _val) = _1]
        | "n\""
            >> lexeme[string            [bind(&Image::fileName, _val) = _1]]
            >> '"'
        | lexeme[string                 [bind(&QStringList::push_back, bind(&Image::unmatchedParameters, _val), _1)]]
    );
}

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* IMAGELINE_DEF_H */
