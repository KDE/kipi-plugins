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

#ifndef OPTIMISATIONLINE_DEF_H
#define OPTIMISATIONLINE_DEF_H

#include "optimisationline.h"

namespace KIPIPanoramaPlugin { namespace PtoParser
{
    struct optimisationParameter_ : qi::symbols<char, PTOType::Optimisation::Parameter>
    {
        optimisationParameter_()
        {
            typedef PTOType::Optimisation O;
            add
            ("a",   O::LENSA)
            ("b",   O::LENSB)
            ("c",   O::LENSC)
            ("d",   O::LENSD)
            ("e",   O::LENSE)
            ("p",   O::LENSP)
            ("r",   O::LENSR)
            ("v",   O::LENSV)
            ("y",   O::LENSY)
            ("Eev", O::EEV)
            ("Er",  O::ER)
            ("Eb",  O::EB)
            ("Va",  O::VA)
            ("Vb",  O::VB)
            ("Vc",  O::VC)
            ("Vd",  O::VD)
            ("Vx",  O::VX)
            ("Vy",  O::VY)
            ("Ra",  O::RA)
            ("Rb",  O::RB)
            ("Rc",  O::RC)
            ("Rd",  O::RD)
            ("Re",  O::RE);
        }
    } optimisationParameter;

    template <typename Iterator>
    optimisationLineGrammar<Iterator>::optimisationLineGrammar() : optimisationLineGrammar::base_type(line)
    {
        using qi::int_;
        using qi::double_;
        using qi::_val;
        using qi::_1;
        using qi::_2;
        using qi::_3;
        using qi::_4;
        using qi::lit;
        using qi::lexeme;
        using phoenix::bind;
        using phoenix::construct;
        using phoenix::push_back;
        typedef PTOType::Optimisation Optimisation;

        // ------------------------- Optimisation line parsing -------------------------

        line =
            optimisationParameter   [bind(&Optimisation::parameter, bind(&OptimisationData::optimisation, _val)) = _1]
         >> int_                    [bind(&OptimisationData::imageId, _val) = _1];
    }

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* OPTIMISATIONLINE_DEF_H */
