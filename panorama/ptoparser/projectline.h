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

#ifndef PROJECTLINE_H
#define PROJECTLINE_H

#include "ptoparser/common.h"

namespace KIPIPanoramaPlugin { namespace PtoParser
{

    template <typename Iterator>
    struct projectLineGrammar : qi::grammar<Iterator, PTOType::Project(), ascii::blank_type>
    {
        projectLineGrammar();

        qi::rule<Iterator, PTOType::Project::FileFormat(), ascii::blank_type> projectFileType;
        qi::rule<Iterator, PTOType::Project(), ascii::blank_type> line;
        stringGrammar<Iterator> string;
        rectangleGrammar<Iterator> rectangle;
    };

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* PROJECTLINE_H */
