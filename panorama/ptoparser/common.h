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

#ifndef PTOPARSER_COMMON_H
#define PTOPARSER_COMMON_H

// Fixes building with MSVC on Windows by disabling internal
// KDE_isspace function mapping. Otherwise, compilation with
// boost spirit will error stating that the isspace function
// is not a member of namespace std
#ifdef _MSC_VER
#ifndef KDEWIN_CTYPE_H
#define KDEWIN_CTYPE_H
#endif
#endif

// Qt includes

#include <QString>
#include <QRect>

// Boost includes

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
// #include <boost/spirit/home/phoenix/bind/bind_function.hpp>
// #include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
// #include <boost/spirit/home/phoenix/bind/bind_member_variable.hpp>
// #include <boost/spirit/home/phoenix/object/construct.hpp>

// #include <boost/spirit/iterator/file_iterator.hpp>

#include "ptotype.h"
#include "boost_adapt.h"

// ----------------------------------------------------------------------------------
// ------ Declaring QString as a suitable container for parsing with boost::qi ------
// ----------------------------------------------------------------------------------
// Found here: http://www.boost.org/doc/libs/1_48_0/libs/spirit/example/qi/custom_string.cpp

namespace boost
{
namespace spirit
{
namespace traits
{
// Make Qi recognize QString as a container
template <> struct is_container<QString> : mpl::true_ {};

// Expose the container's (QString's) value_type
template <> struct container_value<QString> : mpl::identity<QChar> {};

// Define how to insert a new element at the end of the container (QString)
template <>
struct push_back_container<QString, QChar>
{
    static bool call ( QString& c, QChar const& val )
    {
        c.append ( val );
        return true;
    }
};
}
}
} /* namespace boost::spirit::traits */


// ------------------------------------------------
// ------ Some useful aliases and grammar(s) ------
// ------------------------------------------------

namespace KIPIPanoramaPlugin
{
namespace PtoParser
{

namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
typedef std::string::const_iterator iterator_type;

// QString without space
template <typename Iterator>
struct stringGrammar : qi::grammar<Iterator, QString() >
{
    stringGrammar();

    qi::rule<Iterator, QString() > base;
};

template <typename Iterator>
struct rectangleGrammar : qi::grammar<Iterator, QRect() >
{
    rectangleGrammar();

    qi::rule<Iterator, QRect() > base;
};

}
} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* PTOPARSER_COMMON_H */
