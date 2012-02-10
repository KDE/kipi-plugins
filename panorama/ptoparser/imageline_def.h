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


namespace KIPIPanoramaPlugin { namespace PtoParser
{


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
    typedef PTOType::Image Image;

    // ------------------------- Image line parsing -------------------------

    lensParameterInt =
          int_                          [bind(&Image::LensParameter<int>::value, _val) = _1]
        | '=' >> int_                   [bind(&Image::LensParameter<int>::referenceId, _val) = _1];

    lensParameterDouble =
          int_                          [bind(&Image::LensParameter<double>::value, _val) = _1]
        | '=' >> double_                [bind(&Image::LensParameter<double>::referenceId, _val) = _1];

    line = *(
          'w' >> int_                   [bind(&QSize::setWidth, bind(&Image::size, _val), _1)]
        | 'h' >> int_                   [bind(&QSize::setHeight, bind(&Image::size, _val), _1)]
        | 'f' >> int_                   [bind(&Image::lensProjection, _val) = construct<Image::LensProjection>(_1)]
        | 'v' >> lensParameterDouble    [bind(&Image::fieldOfView, _val) = _1]
        | 'y' >> double_                [bind(&Image::yaw, _val) = _1]
        | 'p' >> double_                [bind(&Image::pitch, _val) = _1]
        | 'r' >> double_                [bind(&Image::roll, _val) = _1]
        | 'a' >> lensParameterDouble    [bind(&Image::lensBarrelCoefficientA, _val) = _1]
        | 'b' >> lensParameterDouble    [bind(&Image::lensBarrelCoefficientB, _val) = _1]
        | 'c' >> lensParameterDouble    [bind(&Image::lensBarrelCoefficientC, _val) = _1]
        | 'd' >> lensParameterInt       [bind(&Image::lensCenterOffsetX, _val) = _1]
        | 'e' >> lensParameterInt       [bind(&Image::lensCenterOffsetY, _val) = _1]
        | 'g' >> lensParameterInt       [bind(&Image::lensShearX, _val) = _1]
        | 't' >> lensParameterInt       [bind(&Image::lensShearY, _val) = _1]
        | "Eev" >> double_              [bind(&Image::exposure, _val) = _1]
        | "Er" >> double_               [bind(&Image::whiteBalanceRed, _val) = _1]
        | "Eb" >> double_               [bind(&Image::whiteBalanceBlue, _val) = _1]
        | "Vm" >> int_                  [bind(&Image::vignettingMode, _val) = construct<Image::VignettingMode>(_1)]
        | "Va" >> lensParameterDouble   [bind(&Image::vignettingCorrectionI, _val) = _1]
        | "Vb" >> lensParameterDouble   [bind(&Image::vignettingCorrectionJ, _val) = _1]
        | "Vc" >> lensParameterDouble   [bind(&Image::vignettingCorrectionK, _val) = _1]
        | "Vd" >> lensParameterDouble   [bind(&Image::vignettingCorrectionL, _val) = _1]
        | "Vx" >> lensParameterInt      [bind(&Image::vignettingOffsetX, _val) = _1]
        | "Vy" >> lensParameterInt      [bind(&Image::vignettingOffsetY, _val) = _1]
        | ("Vf\"" >> string >> '"')     [bind(&Image::vignettingFlatfieldImageName, _val) = _1]
        | "Ra" >> lensParameterDouble   [bind(&Image::photometricEMoRA, _val) = _1]
        | "Rb" >> lensParameterDouble   [bind(&Image::photometricEMoRB, _val) = _1]
        | "Rc" >> lensParameterDouble   [bind(&Image::photometricEMoRC, _val) = _1]
        | "Rd" >> lensParameterDouble   [bind(&Image::photometricEMoRD, _val) = _1]
        | "Re" >> lensParameterDouble   [bind(&Image::photometricEMoRE, _val) = _1]
        | "TrX" >> int_                 [bind(&Image::mosaicModeOffsetX, _val) = _1]
        | "TrY" >> int_                 [bind(&Image::mosaicModeOffsetY, _val) = _1]
        | "TrZ" >> int_                 [bind(&Image::mosaicModeOffsetZ, _val) = _1]
        | 'S' >> rectangle              [bind(&Image::crop, _val) = _1]
        | 'j' >> int_                   [bind(&Image::stackNumber, _val) = _1]
        | ("n\"" >> string >> '"')      [bind(&Image::fileName, _val) = _1]
        | lexeme[string                 [bind(&QStringList::push_back, bind(&Image::unmatchedParameters, _val), _1)]]
    );
}

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* IMAGELINE_DEF_H */
