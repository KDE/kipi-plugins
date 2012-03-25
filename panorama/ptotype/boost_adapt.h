/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-03-19
 * Description : a plugin to create panorama by fusion of several images.
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

// Boost includes

#include <boost/fusion/include/adapt_struct.hpp>

// Local includes

#include "ptotype.h"


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
    (double, fieldOfView)
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
    KIPIPanoramaPlugin::PTOType::Mask,
    (QStringList, previousComments)
    (KIPIPanoramaPlugin::PTOType::Mask::MaskType, type)
    (QList<QPoint>, hull)
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
    (QString, fileName)
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