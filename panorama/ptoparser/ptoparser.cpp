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

#include "ptoparser.h"

#include <QFile>

#include <kdebug.h>

#include <string>
#include <iostream>

#include "common.h"
#include "controlpointline.h"
#include "imageline.h"
#include "optimisationline.h"
#include "projectline.h"
#include "variousgrammar.h"


#ifdef BOOST_NO_EXCEPTIONS
namespace boost {
    void throw_exception(std::exception const &e) {kDebug() << "Boost exception" << QString(e.what());}
}
#endif

namespace KIPIPanoramaPlugin { namespace PtoParser
{

// ------------------------------------------------------------
// ------------------------- Grammar --------------------------
// ------------------------------------------------------------

    template <typename Iterator>
    struct ptoGrammar : qi::grammar<Iterator, PTOType(), ascii::blank_type>
    {
        ptoGrammar() : ptoGrammar::base_type(start)
        {
            using ascii::char_;
            using ascii::space;
            using qi::eol;
            using qi::_val;
            using qi::_1;
            using qi::skip;
            using qi::no_skip;
            using phoenix::bind;

            // ------------------------- PTO file parsing -------------------------
            start = no_skip[*((
                      'p' >> skip[projectLine]      [bind(&PTOType::setProject, _val, _1)]
                    | 'm' >> skip[stitcherLine]     [bind(&PTOType::setStitcher, _val, _1)]
                    | 'i' >> skip[imageLine]        [bind(&PTOType::addImage, _val, _1)]
                    | 'k' >> skip[maskLine]         [bind(&PTOType::addMask, _val, bind(&MaskData::mask, _1), bind(&MaskData::imageId, _1))]
                    | 'o' >> skip[optimisationLine] [bind(&PTOType::addOptimisation, _val, bind(&OptimisationData::optimisation, _1), bind(&OptimisationData::imageId, _1))]
                    | 'c' >> skip[controlPointLine] [bind(&PTOType::addControlPoint, _val, _1)]
                    | commentLine                   [bind(&QStringList::push_back, bind(&PTOType::lastComments, _val), _1)]
                )
                >> +eol)]
            ;
        }

        commentLineGrammar<Iterator> commentLine;
        stringGrammar<Iterator> string;
        projectLineGrammar<Iterator> projectLine;
        stitcherLineGrammar<Iterator> stitcherLine;
        maskLineGrammar<Iterator> maskLine;
        imageLineGrammar<Iterator> imageLine;
        optimisationLineGrammar<Iterator> optimisationLine;
        controlPointLineGrammar<Iterator> controlPointLine;
        qi::rule<Iterator, PTOType(), ascii::blank_type> start;
    };
} /* namespace PtoParser */



// --------------------------------------------------------------
// ------------------------- Functions --------------------------
// --------------------------------------------------------------

struct PTOParser::PTOParserPriv
{
    static const PtoParser::ptoGrammar<PtoParser::iterator_type> ptoParser;
};

const PtoParser::ptoGrammar<PtoParser::iterator_type> PTOParser::PTOParserPriv::ptoParser;

PTOParser::PTOParser()
{
}


bool PTOParser::parseFile(const QString& path, PTOType& data)
{
    QFile pto(path);
    if (!pto.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        kDebug() << "PTO File" << path << "cannot be opened for reading.";
        return false;
    }

    QTextStream pto_stream(&pto);
    QString fileQString = pto_stream.readAll();
    pto.close();

    // This is far from being optimal, but QString::const_iterator is not compatible with boost::spirit...

    std::string fileString;
    fileString.append(fileQString.size(), ' ');
    for (int i = 0; i < fileQString.size(); i++)
    {
        fileString[i] = fileQString.at(i).toAscii();
    }

    PtoParser::iterator_type iter = fileString.begin();
    PtoParser::iterator_type last = fileString.end();

    kDebug() << last - iter;
//     bool result = boost::spirit::qi::parse(iter, last, PTOParserPriv::ptoParser, data);
    bool result = boost::spirit::qi::phrase_parse(iter, last, PTOParserPriv::ptoParser, boost::spirit::ascii::blank, data);

    kDebug() << last - iter;

    PtoParser::iterator_type tmp = iter;

    if (tmp != last)
    {
        kDebug() << "Next characters not parsed: ";
        for (int i = 0; i < 20; ++i)
        {
            kDebug() << QChar(*tmp);
            tmp++;
        }
    }

    if (data.project.unmatchedParameters.size() > 0)
    {
        kDebug() << "Project unmatched parameters:";
        foreach (QString s, data.project.unmatchedParameters)
            kDebug() << " -" << s;
    }
    if (data.stitcher.unmatchedParameters.size() > 0)
    {
        kDebug() << "Stitcher unmatched parameters:";
        foreach (QString s, data.stitcher.unmatchedParameters)
            kDebug() << " -" << s;
    }
    foreach (PTOType::Image i, data.images)
    {
        if (i.unmatchedParameters.size() > 0)
        {
            kDebug() << "Image" << i.fileName << "unmatched paramaters:";
            foreach (QString s, i.unmatchedParameters)
                kDebug() << " -" << s;
        }
    }

    foreach (QString s, data.lastComments)
    {
        kDebug() << "Comment:" << s;
    }

    return result && iter == last;
}


} // namespace KIPIPanoramaPlugin
