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

#ifndef COMMON_DEF_H
#define COMMON_DEF_H

#include "common.h"


namespace KIPIPanoramaPlugin { namespace PtoParser
{

    template <typename Iterator>
    stringGrammar<Iterator>::stringGrammar() : stringGrammar::base_type(base)
    {
        using ascii::char_;
        using qi::_val;
        using qi::_1;
        using phoenix::push_back;

        // ------------------------- String without space parsing -------------------------

        base = +(
            (char_ - (ascii::space | '"'))      [push_back(_val,_1)]
        );
    }

    template <typename Iterator>
    rectangleGrammar<Iterator>::rectangleGrammar() : rectangleGrammar::base_type(base)
    {
        using qi::_val;
        using qi::_1;
        using qi::_2;
        using qi::_3;
        using qi::_4;
        using qi::int_;
        using phoenix::construct;
        using phoenix::bind;
        typedef PTOType::Project Project;

        // ------------------------- QRect parsing -------------------------

        base = (int_ >> ',' >> int_ >> ',' >> int_ >> ',' >> int_)
            [_val = construct<QRect>(construct<QPoint>(_1,_3), construct<QSize>(_2 - _1, _4 - _3))];
    }

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* COMMON_DEF_H */
