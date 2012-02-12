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

#include <QString>
#include <QRect>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/bind/bind_function.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_variable.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

// #include <boost/spirit/iterator/file_iterator.hpp>

#include "ptotype.h"


// --------------------------------------
// ------ Boost exceptions thrower ------
// --------------------------------------

#ifdef BOOST_NO_EXCEPTIONS
    void throw_exception(std::exception const & /* e */) {} // user defined
#else
    template<class E> void throw_exception(E const & e)
    {
        throw e;
    }
#endif


// ----------------------------------------------------
// ------ Declaring our structures for boost::qi ------
// ----------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType::Project::FileFormat,
    (KIPIPanoramaPlugin::PTOType::Project::FileFormat::FileType, fileType)
    (unsigned char, quality)
    (KIPIPanoramaPlugin::PTOType::Project::FileFormat::CompressionMethod, compressionMethod)
    (bool, cropped)
    (bool, savePositions)
)

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType::Project,
    (QStringList, previousComments)
    (QSize, size)
    (KIPIPanoramaPlugin::PTOType::Project::ProjectionType, projection)
    (unsigned int, fieldOfView)
    (KIPIPanoramaPlugin::PTOType::Project::FileFormat, fileFormat)
    (double, exposure)
    (bool, hdr)
    (KIPIPanoramaPlugin::PTOType::Project::BitDepth, bitDepth)
    (QRect, crop)
    (int, photometricReferenceId)
    (QStringList, unmatchedParameters)
)

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType::Stitcher,
    (QStringList, previousComments)
    (double, gamma)
    (KIPIPanoramaPlugin::PTOType::Stitcher::Interpolator, interpolator)
    (QStringList, unmatchedParameters)
)

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType::Optimisation,
    (QStringList, previousComments)
    (KIPIPanoramaPlugin::PTOType::Optimisation::Parameter, parameter)
)

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType::ControlPoint,
    (QStringList, previousComments)
    (int, image1Id)
    (int, image2Id)
    (QPoint, p1)
    (QPoint, p2)
    (int, t)
    (QStringList, unmatchedParameters)
)

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType::Image,
    (QStringList, previousComments)
    (QSize, size)
    (int, id)
    (QList<KIPIPanoramaPlugin::PTOType::Mask>, masks)
    (QList<KIPIPanoramaPlugin::PTOType::Optimisation>, optimisationParameters)
    (KIPIPanoramaPlugin::PTOType::Image::LensProjection, lensProjection)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, fieldOfView)
    (double, yaw)
    (double, pitch)
    (double, roll)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, lensBarrelCoefficientA)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, lensBarrelCoefficientB)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, lensBarrelCoefficientC)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<int>, lensCenterOffsetX)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<int>, lensCenterOffsetY)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<int>, lensShearX)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<int>, lensShearY)
    (double, exposure)
    (double, whiteBalanceRed)
    (double, whiteBalanceBlue)
    (KIPIPanoramaPlugin::PTOType::Image::VignettingMode, vignettingMode)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, vignettingCorrectionI)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, vignettingCorrectionJ)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, vignettingCorrectionK)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, vignettingCorrectionL)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<int>, vignettingOffsetX)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<int>, vignettingOffsetY)
    (QString, vignettingFlatfieldImageName)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, photometricEMoRA)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, photometricEMoRB)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, photometricEMoRC)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, photometricEMoRD)
    (KIPIPanoramaPlugin::PTOType::Image::LensParameter<double>, photometricEMoRE)
    (int, mosaicModeOffsetX)
    (int, mosaicModeOffsetY)
    (int, mosaicModeOffsetZ)
    (QRect, crop)
    (int, stackNumber)
    (QString, filename)
    (QStringList, unmatchedParameters)
)

BOOST_FUSION_ADAPT_STRUCT(
    KIPIPanoramaPlugin::PTOType,
    (KIPIPanoramaPlugin::PTOType::Project, project)
    (KIPIPanoramaPlugin::PTOType::Stitcher, stitcher)
    (QVector<KIPIPanoramaPlugin::PTOType::Image>, images)
    (QList<KIPIPanoramaPlugin::PTOType::ControlPoint>, controlPoints)
    (QStringList, lastComments)
)

// ----------------------------------------------------------------------------------
// ------ Declaring QString as a suitable container for parsing with boost::qi ------
// ----------------------------------------------------------------------------------
// Found here: http://www.boost.org/doc/libs/1_48_0/libs/spirit/example/qi/custom_string.cpp

namespace boost { namespace spirit { namespace traits
{
    // Make Qi recognize QString as a container
    template <> struct is_container<QString> : mpl::true_ {};

    // Expose the container's (QString's) value_type
    template <> struct container_value<QString> : mpl::identity<QChar> {};

    // Define how to insert a new element at the end of the container (QString)
    template <>
    struct push_back_container<QString, QChar>
    {
        static bool call(QString& c, QChar const& val)
        {
            c.append(val);
            return true;
        }
    };
}}} /* namespace boost::spirit::traits */


// ------------------------------------------------
// ------ Some useful aliases and grammar(s) ------
// ------------------------------------------------

namespace KIPIPanoramaPlugin { namespace PtoParser
{

    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    typedef std::string::const_iterator iterator_type;

    // QString without space
    template <typename Iterator>
    struct stringGrammar : qi::grammar<Iterator, QString()>
    {
        stringGrammar();

        qi::rule<Iterator, QString()> base;
    };

    template <typename Iterator>
    struct rectangleGrammar : qi::grammar<Iterator, QRect()>
    {
        rectangleGrammar();

        qi::rule<Iterator, QRect()> base;
    };

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* PTOPARSER_COMMON_H */
