/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-04
 * Description : a plugin to create panorama by fusion of several images.
 *               This type is based on pto file format described here:
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

#ifndef PTOTYPE_H
#define PTOTYPE_H

#include <QPoint>
#include <QVector>
#include <QSize>
#include <QString>
#include <QRect>
#include <QStringList>
#include <QPair>
#include <QTextStream>

namespace KIPIPanoramaPlugin
{

struct PTOType
{
    struct Project
    {
        struct FileFormat {
            typedef enum { PNG, TIFF, TIFF_m, TIFF_multilayer, JPEG } FileType;
            typedef enum { NONE, LZW, DEFLATE } CompressionMethod;

            FileType                    fileType;
            unsigned char               quality;        // JPEG
            CompressionMethod           compressionMethod;         // TIFF
            bool                        cropped;            // TIFF
            bool                        savePositions;      // TIFF
        };

        typedef enum {
            RECTILINEAR = 0,
            CYLINDRICAL = 1,
            EQUIRECTANGULAR = 2,
            FULLFRAMEFISHEYE = 3
        } ProjectionType;

        typedef enum { UINT8, UINT16, FLOAT } BitDepth;

        QStringList                 previousComments;
        QSize                       size;
        QRect                       crop;
        ProjectionType              projection;
        double                      fieldOfView;
        FileFormat                  fileFormat;
        double                      exposure;
        bool                        hdr;
        BitDepth                    bitDepth;
        int                         photometricReferenceId;
        QStringList                 unmatchedParameters;
    };

    struct Stitcher
    {
        typedef enum {
            POLY3 = 0,
            SPLINE16 = 1,
            SPLINE36 = 2,
            SINC256 = 3,
            SPLINE64 = 4,
            BILINEAR = 5,
            NEARESTNEIGHBOR = 6,
            SINC1024 = 7
        } Interpolator;

        typedef enum { SLOW, MEDIUM, FAST } SpeedUp;

        QStringList                 previousComments;
        double                      gamma;
        Interpolator                interpolator;
        SpeedUp                     speedUp;
        double                      huberSigma;
        double                      photometricHuberSigma;
        QStringList                 unmatchedParameters;
    };

    struct Mask
    {
        typedef enum {
            NEGATIVE = 0,
            POSTIVE = 1,
            NEGATIVESTACK = 2,
            POSITIVESTACK = 3,
            NEGATIVELENS = 4
        } MaskType;

        QStringList                 previousComments;
        MaskType                    type;
        QList<QPoint>               hull;
    };

    struct Optimisation
    {
        typedef enum {
            LENSA, LENSB, LENSC, LENSD, LENSE, LENSHFOV, LENSYAW, LENSPITCH, LENSROLL,
            EXPOSURE, WBR, WBB,
            VA, VB, VC, VD, VX, VY,
            RA, RB, RC, RD, RE,
            UNKNOWN
        } Parameter;

        QStringList                 previousComments;
        Parameter                   parameter;
    };

    struct Image
    {
        template<typename T>
        struct LensParameter {
            LensParameter() { referenceId = -1; }

            T           value;
            int         referenceId;

            friend
            QTextStream& operator<<(QTextStream& qts, const LensParameter<T>& p)
            {
               if (p.referenceId == -1)
                   qts << p.value;
               else
                   qts << "=" << p.referenceId;

                return qts;
            }
        };

        typedef enum {
            RECTILINEAR = 0,
            PANORAMIC = 1,
            CIRCULARFISHEYE = 2,
            FULLFRAMEFISHEYE = 3,
            EQUIRECTANGULAR = 4
        } LensProjection;

        typedef enum {
            NONE = 0,
            RADIAL = 1,
            FLATFIELD = 2,
            PROPORTIONNALRADIAL = 5,
            PROPORTIONNALFLATFIELD = 6
        } VignettingMode;

        QStringList                     previousComments;
        QSize                           size;
        int                             id;
        QList<Mask>                     masks;
        QList<Optimisation>             optimisationParameters;
        LensProjection                  lensProjection;
        LensParameter<double>           fieldOfView;
        double                          yaw;
        double                          pitch;
        double                          roll;
        LensParameter<double>           lensBarrelCoefficientA;
        LensParameter<double>           lensBarrelCoefficientB;
        LensParameter<double>           lensBarrelCoefficientC;
        LensParameter<int>              lensCenterOffsetX;
        LensParameter<int>              lensCenterOffsetY;
        LensParameter<int>              lensShearX;
        LensParameter<int>              lensShearY;
        LensParameter<double>           exposure;
        LensParameter<double>           whiteBalanceRed;
        LensParameter<double>           whiteBalanceBlue;
        LensParameter<VignettingMode>   vignettingMode;
        LensParameter<double>           vignettingCorrectionI;      // Va
        LensParameter<double>           vignettingCorrectionJ;      // Vb
        LensParameter<double>           vignettingCorrectionK;      // Vc
        LensParameter<double>           vignettingCorrectionL;      // Vd
        LensParameter<int>              vignettingOffsetX;
        LensParameter<int>              vignettingOffsetY;
        QString                         vignettingFlatfieldImageName;
        LensParameter<double>           photometricEMoRA;
        LensParameter<double>           photometricEMoRB;
        LensParameter<double>           photometricEMoRC;
        LensParameter<double>           photometricEMoRD;
        LensParameter<double>           photometricEMoRE;
        int                             mosaicModeOffsetX;
        int                             mosaicModeOffsetY;
        int                             mosaicModeOffsetZ;
        QRect                           crop;
        LensParameter<int>              stackNumber;
        QString                         fileName;
        QStringList                     unmatchedParameters;
    };

    struct ControlPoint
    {
        QStringList                 previousComments;
        int                         image1Id;
        int                         image2Id;
        double                      p1_x;
        double                      p1_y;
        double                      p2_x;
        double                      p2_y;
        int                         type;   // FIXME: change that for an enum if possible
        QStringList                 unmatchedParameters;
    };

    bool                createFile(const QString& filepath);

    /* NOTE: Work in progress
    QPair<double, int>  standardDeviation(int image1Id, int image2Id);
    QPair<double, int>  standardDeviation(int imageId);
    QPair<double, int>  standardDeviation();
    */

    Project                 project;
    Stitcher                stitcher;
    QVector<Image>          images;
    QList<ControlPoint>     controlPoints;
    QStringList             lastComments;
};

} // namespace KIPIPanoramaPlugin

#endif /* PTOTYPE_H */
