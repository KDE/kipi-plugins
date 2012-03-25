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

#ifndef VARIOUSGRAMMAR_H
#define VARIOUSGRAMMAR_H

#include "ptoparser/common.h"

// ----------------------------------
// ------ Some useful grammars ------
// ----------------------------------

namespace KIPIPanoramaPlugin { namespace PtoParser
{

    struct MaskData {
        PTOType::Mask   mask;
        int             imageId;
    };

}} /* namespace KIPIPanoramaPlugin::PtoParser */

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PtoParser::MaskData,
    (KIPIPanoramaPlugin::PTOType::Mask, mask)
    (int, imageId)
)

namespace KIPIPanoramaPlugin { namespace PtoParser
{

    // Whole line
    template <typename Iterator>
    struct commentLineGrammar : qi::grammar<Iterator, QString()>
    {
        commentLineGrammar();

        qi::rule<Iterator, QString()> base;
    };

    template <typename Iterator>
    struct stitcherLineGrammar : qi::grammar<Iterator, PTOType::Stitcher(), ascii::blank_type>
    {
        stitcherLineGrammar();

        qi::rule<Iterator, PTOType::Stitcher(), ascii::blank_type> base;
        stringGrammar<Iterator> string;
    };

    template <typename Iterator>
    struct maskLineGrammar : qi::grammar<Iterator, MaskData(), ascii::blank_type>
    {
        maskLineGrammar();

        qi::rule<Iterator, MaskData(), ascii::blank_type> base;
    };

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* VARIOUSGRAMMAR_H */
