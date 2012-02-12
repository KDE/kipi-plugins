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

#ifndef CONTROLPOINTLINE_DEF_H
#define CONTROLPOINTLINE_DEF_H

#include "controlpointline.h"


namespace KIPIPanoramaPlugin { namespace PtoParser
{

    template <typename Iterator>
    controlPointLineGrammar<Iterator>::controlPointLineGrammar() : controlPointLineGrammar::base_type(line)
    {
        using qi::int_;
        using qi::double_;
        using qi::_val;
        using qi::_1;
        using qi::lexeme;
        using phoenix::bind;
        using phoenix::push_back;
        typedef PTOType::ControlPoint ControlPoint;

        // ------------------------- Control point line parsing -------------------------

        line = *(
              'n' >> int_               [bind(&ControlPoint::image1Id, _val) = _1]
            | 'N' >> int_               [bind(&ControlPoint::image2Id, _val) = _1]
            | 'x' >> double_            [bind(&QPoint::setX, bind(&ControlPoint::p1, _val), _1)]
            | 'y' >> double_            [bind(&QPoint::setY, bind(&ControlPoint::p1, _val), _1)]
            | 'X' >> double_            [bind(&QPoint::setX, bind(&ControlPoint::p2, _val), _1)]
            | 'Y' >> double_            [bind(&QPoint::setY, bind(&ControlPoint::p2, _val), _1)]
            | lexeme[string             [bind(&QStringList::push_back, bind(&ControlPoint::unmatchedParameters, _val), _1)]]
        );
    }

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* CONTROLPOINTLINE_DEF_H */
