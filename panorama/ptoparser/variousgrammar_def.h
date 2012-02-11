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

#ifndef VARIOUSGRAMMAR_DEF_H
#define VARIOUSGRAMMAR_DEF_H

#include "variousgrammar.h"


namespace KIPIPanoramaPlugin { namespace PtoParser
{

    template <typename Iterator>
    commentLineGrammar<Iterator>::commentLineGrammar() : commentLineGrammar::base_type(base)
    {
        using ascii::char_;
        using qi::_val;
        using qi::_1;
        using qi::eol;
        using phoenix::push_back;

        // ------------------------- Comment parsing -------------------------

        base = +(
            (char_ - eol)              [push_back(_val,_1)]
            );
    }

    template <typename Iterator>
    stitcherLineGrammar<Iterator>::stitcherLineGrammar() : stitcherLineGrammar::base_type(base)
    {
        using qi::_val;
        using qi::_1;
        using qi::double_;
        using qi::int_;
        using phoenix::bind;
        using phoenix::construct;
        typedef PTOType::Stitcher Stitcher;

        // ------------------------- Stitcher line parsing -------------------------

        base = *(
              'g' >> double_            [bind(&Stitcher::gamma, _val) = _1]
            | 'i' >> int_               [bind(&Stitcher::interpolator, _val) = construct<Stitcher::Interpolator>(_1)]
            | string                    [bind(&QStringList::push_back, bind(&Stitcher::unmatchedParameters, _val), _1)]
        );
    }

    template <typename Iterator>
    maskLineGrammar<Iterator>::maskLineGrammar() : maskLineGrammar::base_type(base)
    {
        using qi::_val;
        using qi::_1;
        using qi::_2;
        using qi::int_;
        using phoenix::bind;
        using phoenix::construct;
        typedef PTOType::Mask Mask;
        
        // ------------------------- Mask line parsing -------------------------

        base = *(
                  'i' >> int_               [bind(&MaskData::imageId, _val) = _1]
                | 't' >> int_               [bind(&Mask::type, bind(&MaskData::mask, _val)) = construct<Mask::MaskType>(_1)]
                | ("p\"" >> *(
                    (int_ >> int_)          [bind(&QList<QPoint>::push_back, bind(&Mask::hull, bind(&MaskData::mask, _val) ), construct<QPoint>(_1,_2))]
                    ) >> '"')
            );
    }

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* VARIOUSGRAMMAR_DEF_H */
