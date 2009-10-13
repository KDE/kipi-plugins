/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

/**
    References about DNG:
    DNG SDK tutorial:    http://www.adobeforums.com/webx/.3bc2944e
                         http://www.adobeforums.com/webx/.3c054bde
    DNG review:          http://www.barrypearson.co.uk/articles/dng/index.htm
    DNG intro:           http://www.adobe.com/digitalimag/pdfs/dng_primer.pdf
                         http://www.adobe.com/products/dng/pdfs/DNG_primer_manufacturers.pdf
    DNG Specification:   http://www.adobe.com/products/dng/pdfs/dng_spec_1_2_0_0.pdf
    TIFF/EP Spec.:       http://www.map.tu.chiba-u.ac.jp/IEC/100/TA2/recdoc/N4378.pdf
    DNG SDK reference:   http://www.thomasdideriksen.dk/misc/File%20Formats/dng_sdk_refman.pdf
    DNG SDK tarball:     ftp://ftp.adobe.com/pub/adobe/dng/dng_sdk_1_2.zip
    DNG users forum:     http://www.adobeforums.com/webx/.3bb5f0ec

    Applications using DNG SDK:
    DNG4PS2:             http://dng4ps2.chat.ru/index_en.html
    CORNERFIX:           http://sourceforge.net/projects/cornerfix
    ADOBE DNG CONVERTER: ftp://ftp.adobe.com/pub/adobe/photoshop/cameraraw/win/4.x
*/

// C ansi includes

extern "C"
{
#include <sys/stat.h>
#include <utime.h>
}

// Qt includes

#include <QImage>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QTemporaryFile>

// KDE includes

#include <kdebug.h>
#include <kzip.h>

// Libkexiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "dngwriter_p.h"
#include "dngwriterhost.h"
#include "dngwriter.h"

using namespace KExiv2Iface;

namespace DNGIface
{

DNGWriter::DNGWriter()
         : d(new DNGWriterPrivate)
{
}

DNGWriter::~DNGWriter()
{
    delete d;
}

void DNGWriter::cancel()
{
    d->cancel = true;
}

void DNGWriter::reset()
{
    d->reset();
}

void DNGWriter::setCompressLossLess(bool b)
{
    d->jpegLossLessCompression = b;
}

bool DNGWriter::compressLossLess() const
{
    return d->jpegLossLessCompression;
}

void DNGWriter::setUpdateFileDate(bool b)
{
    d->updateFileDate = b;
}

bool DNGWriter::updateFileDate() const
{
    return d->updateFileDate;
}

void DNGWriter::setBackupOriginalRawFile(bool b)
{
    d->backupOriginalRawFile = b;
}

bool DNGWriter::backupOriginalRawFile() const
{
    return d->backupOriginalRawFile;
}

void DNGWriter::setPreviewMode(int mode)
{
    d->previewMode = mode;
}

int DNGWriter::previewMode() const
{
    return d->previewMode;
}

void DNGWriter::setInputFile(const QString& filePath)
{
    d->inputFile = filePath;
}

void DNGWriter::setOutputFile(const QString& filePath)
{
    d->outputFile = filePath;
}

QString DNGWriter::inputFile() const
{
    return d->inputFile;
}

QString DNGWriter::outputFile() const
{
    return d->outputFile;
}

int DNGWriter::convert()
{
    d->cancel = false;

    try
    {
        if (inputFile().isEmpty())
        {
            kDebug(51000) << "DNGWriter: No input file to convert. Aborted..." ;
            return -1;
        }

        QFileInfo inputInfo(inputFile());
        QString   dngFilePath = outputFile();

        if (dngFilePath.isEmpty())
        {
            dngFilePath = QString(inputInfo.completeBaseName() + QString(".dng"));
        }

        QFileInfo          outputInfo(dngFilePath);
        QByteArray         rawData;
        DcrawInfoContainer identify;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: Loading RAW data from " << inputInfo.fileName() ;

        KDcraw rawProcessor;
        if (!rawProcessor.extractRAWData(inputFile(), rawData, identify))
        {
            kDebug(51000) << "DNGWriter: Loading RAW data failed. Aborted..." ;
            return -1;
        }

        if (d->cancel) return -2;

        int width      = identify.imageSize.width();
        int height     = identify.imageSize.height();
        int pixelRange = 16;

        kDebug(51000) << "DNGWriter: Raw data loaded:" ;
        kDebug(51000) << "--- Data Size:     " << rawData.size() << " bytes" ;
        kDebug(51000) << "--- Date:          " << identify.dateTime.toString(Qt::ISODate) ;
        kDebug(51000) << "--- Make:          " << identify.make ;
        kDebug(51000) << "--- Model:         " << identify.model ;
        kDebug(51000) << "--- Size:          " << width << "x" << height ;
        kDebug(51000) << "--- Orientation:   " << identify.orientation ;
        kDebug(51000) << "--- Top margin:    " << identify.topMargin ;
        kDebug(51000) << "--- Left margin:   " << identify.leftMargin ;
        kDebug(51000) << "--- Filter:        " << identify.filterPattern ;
        kDebug(51000) << "--- Colors:        " << identify.rawColors ;
        kDebug(51000) << "--- Black:         " << identify.blackPoint ;
        kDebug(51000) << "--- White:         " << identify.whitePoint ;
        kDebug(51000) << "--- CAM->XYZ:" ;

        QString matrixVal;
        for(int i=0; i<12; i+=3)
        {
            kDebug(51000) << "                   "
                          << QString().sprintf("%03.4f  %03.4f  %03.4f", identify.cameraXYZMatrix[0][ i ],
                                                                         identify.cameraXYZMatrix[0][i+1],
                                                                         identify.cameraXYZMatrix[0][i+2])
                          ;
        }

        // Check if CFA layout is supported by DNG SDK.
        int bayerMosaic;

        if (identify.filterPattern == QString("GRBGGRBGGRBGGRBG"))
        {
            bayerMosaic = 0;
        }
        else if (identify.filterPattern == QString("RGGBRGGBRGGBRGGB"))
        {
            bayerMosaic = 1;
        }
        else if (identify.filterPattern == QString("BGGRBGGRBGGRBGGR"))
        {
            bayerMosaic = 2;
        }
        else if (identify.filterPattern == QString("GBRGGBRGGBRGGBRG"))
        {
            bayerMosaic = 3;
        }
        else
        {
            kDebug(51000) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;
            return -1;
        }

        // Check if number of Raw Color components is supported.
        if (identify.rawColors != 3)
        {
            kDebug(51000) << "DNGWriter: Number of Raw color components not supported. Aborted..." ;
            return -1;
        }

/*      // NOTE: code to hack RAW data extraction

        QString   rawdataFilePath(inputInfo.completeBaseName() + QString(".dat"));
        QFileInfo rawdataInfo(rawdataFilePath);

        QFile rawdataFile(rawdataFilePath);
        if (!rawdataFile.open(QIODevice::WriteOnly))
        {
            kDebug(51000) << "DNGWriter: Cannot open file to write RAW data. Aborted..." ;
            return -1;
        }
        QDataStream rawdataStream(&rawdataFile);
        rawdataStream.writeRawData(rawData.data(), rawData.size());
        rawdataFile.close();
*/
        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: Formating RAW data to memory" ;

        std::vector<unsigned short> raw_data;
        raw_data.resize(rawData.size());
        const unsigned short* dp = (const unsigned short*)rawData.data();
        for (uint i = 0; i < raw_data.size()/2; i++)
        {
            raw_data[i] = *dp;
            *dp++;
        }

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: DNG memory allocation and initialization" ;

        dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);
        dng_memory_stream stream(memalloc);
        stream.Put(&raw_data.front(), raw_data.size()*sizeof(unsigned short));

        dng_rect rect(height, width);
        DNGWriterHost host(d, &memalloc);

        // Unprocessed raw data.
        host.SetKeepStage1(true);

        // Linearized, tone curve processed data.
        host.SetKeepStage2(true);

        AutoPtr<dng_image> image(new dng_simple_image(rect, 1, ttShort, 1<<pixelRange, memalloc));

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: DNG IFD structure creation" ;

        dng_ifd ifd;

        ifd.fUsesNewSubFileType        = true;
        ifd.fNewSubFileType            = 0;
        ifd.fImageWidth                = width;
        ifd.fImageLength               = height;
        ifd.fBitsPerSample[0]          = pixelRange;
        ifd.fBitsPerSample[1]          = 0;
        ifd.fBitsPerSample[2]          = 0;
        ifd.fBitsPerSample[3]          = 0;
        ifd.fCompression               = ccUncompressed;
        ifd.fPredictor                 = 1;
        ifd.fCFALayout                 = 1;                 // Rectangular (or square) layout.
        ifd.fPhotometricInterpretation = piCFA;
        ifd.fFillOrder                 = 1;
        ifd.fOrientation               = identify.orientation;
        ifd.fSamplesPerPixel           = 1;
        ifd.fPlanarConfiguration       = 1;
        ifd.fXResolution               = 0.0;
        ifd.fYResolution               = 0.0;
        ifd.fResolutionUnit            = 0;

        ifd.fUsesStrips                = true;
        ifd.fUsesTiles                 = false;

        ifd.fTileWidth                 = width;
        ifd.fTileLength                = height;
        ifd.fTileOffsetsType           = 4;
        ifd.fTileOffsetsCount          = 0;

        ifd.fSubIFDsCount              = 0;
        ifd.fSubIFDsOffset             = 0;
        ifd.fExtraSamplesCount         = 0;
        ifd.fSampleFormat[0]           = 1;
        ifd.fSampleFormat[1]           = 1;
        ifd.fSampleFormat[2]           = 1;
        ifd.fSampleFormat[3]           = 1;

        ifd.fLinearizationTableType    = 0;
        ifd.fLinearizationTableCount   = 0;
        ifd.fLinearizationTableOffset  = 0;

        ifd.fBlackLevelRepeatRows      = 1;
        ifd.fBlackLevelRepeatCols      = 1;
        ifd.fBlackLevel[0][0][0]       = identify.blackPoint;
        ifd.fBlackLevelDeltaHType      = 0;
        ifd.fBlackLevelDeltaHCount     = 0;
        ifd.fBlackLevelDeltaHOffset    = 0;
        ifd.fBlackLevelDeltaVType      = 0;
        ifd.fBlackLevelDeltaVCount     = 0;
        ifd.fBlackLevelDeltaVOffset    = 0;
        ifd.fWhiteLevel[0]             = identify.whitePoint;
        ifd.fWhiteLevel[1]             = identify.whitePoint;
        ifd.fWhiteLevel[2]             = identify.whitePoint;
        ifd.fWhiteLevel[3]             = identify.whitePoint;

        ifd.fDefaultScaleH             = dng_urational(1, 1);
        ifd.fDefaultScaleV             = dng_urational(1, 1);
        ifd.fBestQualityScale          = dng_urational(1, 1);

        ifd.fCFARepeatPatternRows      = 0;
        ifd.fCFARepeatPatternCols      = 0;

        ifd.fBayerGreenSplit           = 0;
        ifd.fChromaBlurRadius          = dng_urational(0, 0);
        ifd.fAntiAliasStrength         = dng_urational(100, 100);

        ifd.fActiveArea                = rect;
        ifd.fDefaultCropOriginH        = dng_urational(0, 1);
        ifd.fDefaultCropOriginV        = dng_urational(0, 1);
        ifd.fDefaultCropSizeH          = dng_urational(width, 1);
        ifd.fDefaultCropSizeV          = dng_urational(height, 1);

        ifd.fMaskedAreaCount           = 0;
        ifd.fLosslessJPEGBug16         = false;
        ifd.fSampleBitShift            = 0;

        ifd.ReadImage(host, stream, *image.Get());

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: DNG Negative structure creation" ;

        AutoPtr<dng_negative> negative(host.Make_dng_negative());

        negative->SetDefaultScale(ifd.fDefaultScaleH, ifd.fDefaultScaleV);
        negative->SetDefaultCropOrigin(ifd.fDefaultCropOriginH, ifd.fDefaultCropOriginV);
        negative->SetDefaultCropSize(ifd.fDefaultCropSizeH, ifd.fDefaultCropSizeV);
        negative->SetActiveArea(ifd.fActiveArea);

        negative->SetModelName(identify.model.toAscii());
        negative->SetLocalName(QString("%1 %2").arg(identify.make).arg(identify.model).toAscii());
        negative->SetOriginalRawFileName(inputInfo.fileName().toAscii());

        negative->SetColorChannels(3);
        negative->SetColorKeys(colorKeyRed, colorKeyGreen, colorKeyBlue);
        negative->SetBayerMosaic(bayerMosaic);

        negative->SetWhiteLevel(identify.whitePoint, 0);
        negative->SetWhiteLevel(identify.whitePoint, 1);
        negative->SetWhiteLevel(identify.whitePoint, 2);
        negative->SetBlackLevel(identify.blackPoint, 0);
        negative->SetBlackLevel(identify.blackPoint, 1);
        negative->SetBlackLevel(identify.blackPoint, 2);

        negative->SetBaselineExposure(0.0);
        negative->SetBaselineNoise(1.0);
        negative->SetBaselineSharpness(1.0);

        dng_orientation orientation;
        switch (identify.orientation)
        {
            case DcrawInfoContainer::ORIENTATION_180:
                orientation = dng_orientation::Rotate180();
                break;

            case DcrawInfoContainer::ORIENTATION_90CCW:
                orientation = dng_orientation::Rotate90CCW();
                break;

            case DcrawInfoContainer::ORIENTATION_90CW:
                orientation = dng_orientation::Rotate90CW();
                break;

            default:   // ORIENTATION_NONE
                orientation = dng_orientation::Normal();
                break;
        }
        negative->SetBaseOrientation(orientation);

        negative->SetAntiAliasStrength(dng_urational(100, 100));
        negative->SetLinearResponseLimit(1.0);
        negative->SetShadowScale( dng_urational(1, 1) );

        negative->SetAnalogBalance(dng_vector_3(1.0, 1.0, 1.0));

        // -------------------------------------------------------------------------------

        // Set Camera->XYZ Color matrix as profile.
        dng_matrix_3by3 matrix(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
        dng_matrix_3by3 camXYZ;

        AutoPtr<dng_camera_profile> prof(new dng_camera_profile);
        prof->SetName(QString("%1 %2").arg(identify.make).arg(identify.model).toAscii());

        camXYZ[0][0] = identify.cameraXYZMatrix[0][0];
        camXYZ[0][1] = identify.cameraXYZMatrix[0][1];
        camXYZ[0][2] = identify.cameraXYZMatrix[0][2];
        camXYZ[1][0] = identify.cameraXYZMatrix[0][3];
        camXYZ[1][1] = identify.cameraXYZMatrix[1][0];
        camXYZ[1][2] = identify.cameraXYZMatrix[1][1];
        camXYZ[2][0] = identify.cameraXYZMatrix[1][2];
        camXYZ[2][1] = identify.cameraXYZMatrix[1][3];
        camXYZ[2][2] = identify.cameraXYZMatrix[2][0];

        if (camXYZ.MaxEntry() == 0.0)
            kDebug(51000) << "DNGWriter: Warning, camera XYZ Matrix is null" ;
        else
            matrix = camXYZ;

        prof->SetColorMatrix1((dng_matrix) matrix);
        prof->SetCalibrationIlluminant1(lsD65);
        negative->AddProfile(prof);

        // -------------------------------------------------------------------------------

        // Clear "Camera WhiteXY"
        negative->SetCameraWhiteXY(dng_xy_coord());

        // This settings break color on preview and thumbnail
        //negative->SetCameraNeutral(dng_vector_3(1.0, 1.0, 1.0));

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: Updating metadata to DNG Negative" ;

        dng_exif *exif = negative->GetExif();
        exif->fModel.Set_ASCII(identify.model.toAscii());
        exif->fMake.Set_ASCII(identify.make.toAscii());

        // Time from original shot
        dng_date_time dt;
        dt.fYear   = identify.dateTime.date().year();
        dt.fMonth  = identify.dateTime.date().month();
        dt.fDay    = identify.dateTime.date().day();
        dt.fHour   = identify.dateTime.time().hour();
        dt.fMinute = identify.dateTime.time().minute();
        dt.fSecond = identify.dateTime.time().second();

        dng_date_time_info dti;
        dti.SetDateTime(dt);
        exif->fDateTimeOriginal  = dti;
        exif->fDateTimeDigitized = dti;
        negative->UpdateDateTime(dti);

        long int num, den;
        long     val;
        QString  str;
        KExiv2   meta;
        if (meta.load(inputFile()))
        {
            // String Tags

            str = meta.getExifTagString("Exif.Image.Software");
            if (!str.isEmpty()) exif->fSoftware.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.Image.ImageDescription");
            if (!str.isEmpty()) exif->fImageDescription.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.Image.Artist");
            if (!str.isEmpty()) exif->fArtist.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.Image.Copyright");
            if (!str.isEmpty()) exif->fCopyright.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.Photo.UserComment");
            if (!str.isEmpty()) exif->fUserComment.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.Image.CameraSerialNumber");
            if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSLatitudeRef");
            if (!str.isEmpty()) exif->fGPSLatitudeRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSLongitudeRef");
            if (!str.isEmpty()) exif->fGPSLongitudeRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSSatellites");
            if (!str.isEmpty()) exif->fGPSSatellites.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSStatus");
            if (!str.isEmpty()) exif->fGPSStatus.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSMeasureMode");
            if (!str.isEmpty()) exif->fGPSMeasureMode.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSSpeedRef");
            if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSTrackRef");
            if (!str.isEmpty()) exif->fGPSTrackRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSSpeedRef");
            if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSImgDirectionRef");
            if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSMapDatum");
            if (!str.isEmpty()) exif->fGPSMapDatum.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSDestLatitudeRef");
            if (!str.isEmpty()) exif->fGPSDestLatitudeRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSDestLongitudeRef");
            if (!str.isEmpty()) exif->fGPSDestLongitudeRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSDestBearingRef");
            if (!str.isEmpty()) exif->fGPSDestBearingRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSDestDistanceRef");
            if (!str.isEmpty()) exif->fGPSDestDistanceRef.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSProcessingMethod");
            if (!str.isEmpty()) exif->fGPSProcessingMethod.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSAreaInformation");
            if (!str.isEmpty()) exif->fGPSAreaInformation.Set_ASCII(str.toAscii());

            str = meta.getExifTagString("Exif.GPSInfo.GPSDateStamp");
            if (!str.isEmpty()) exif->fGPSDateStamp.Set_ASCII(str.toAscii());

            // Rational Tags

            if (meta.getExifTagRational("Exif.Photo.ExposureTime", num, den))          exif->fExposureTime             = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.FNumber", num, den))               exif->fFNumber                  = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.ShutterSpeedValue", num, den))     exif->fShutterSpeedValue        = dng_srational(num, den);
            if (meta.getExifTagRational("Exif.Photo.ApertureValue", num, den))         exif->fApertureValue            = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.BrightnessValue", num, den))       exif->fBrightnessValue          = dng_srational(num, den);
            if (meta.getExifTagRational("Exif.Photo.ExposureBiasValue", num, den))     exif->fExposureBiasValue        = dng_srational(num, den);
            if (meta.getExifTagRational("Exif.Photo.MaxApertureValue", num, den))      exif->fMaxApertureValue         = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.FocalLength", num, den))           exif->fFocalLength              = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.DigitalZoomRatio", num, den))      exif->fDigitalZoomRatio         = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.SubjectDistance", num, den))       exif->fSubjectDistance          = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Image.BatteryLevel", num, den))          exif->fBatteryLevelR            = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.FocalPlaneXResolution", num, den)) exif->fFocalPlaneXResolution    = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.Photo.FocalPlaneYResolution", num, den)) exif->fFocalPlaneYResolution    = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSAltitude", num, den))         exif->fGPSAltitude              = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSDOP", num, den))              exif->fGPSDOP                   = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSSpeed", num, den))            exif->fGPSSpeed                 = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSTrack", num, den))            exif->fGPSTrack                 = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSImgDirection", num, den))     exif->fGPSImgDirection          = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSDestBearing", num, den))      exif->fGPSDestBearing           = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSDestDistance", num, den))     exif->fGPSDestDistance          = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den))         exif->fGPSLatitude[0]           = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den))        exif->fGPSLongitude[0]          = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den))        exif->fGPSTimeStamp[0]          = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den))     exif->fGPSDestLatitude[0]       = dng_urational(num, den);
            if (meta.getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den))    exif->fGPSDestLongitude[0]      = dng_urational(num, den);

            // Integer Tags

            if (meta.getExifTagLong("Exif.Photo.ExposureProgram", val))                exif->fExposureProgram          = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.MeteringMode", val))                   exif->fMeteringMode             = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.LightSource", val))                    exif->fLightSource              = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.Flash", val))                          exif->fFlash                    = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.SensingMethod", val))                  exif->fSensingMethod            = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.FileSource", val))                     exif->fFileSource               = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.SceneType", val))                      exif->fSceneType                = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.CustomRendered", val))                 exif->fCustomRendered           = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.ExposureMode", val))                   exif->fExposureMode             = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.WhiteBalance", val))                   exif->fWhiteBalance             = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.SceneCaptureType", val))               exif->fSceneCaptureType         = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.GainControl", val))                    exif->fGainControl              = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.Contrast", val))                       exif->fContrast                 = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.Saturation", val))                     exif->fSaturation               = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.Sharpness", val))                      exif->fSharpness                = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.SubjectDistanceRange", val))           exif->fSubjectDistanceRange     = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.FocalLengthIn35mmFilm", val))          exif->fFocalLengthIn35mmFilm    = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.ComponentsConfiguration", val))        exif->fComponentsConfiguration  = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.PixelXDimension", val))                exif->fPixelXDimension          = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.PixelYDimension", val))                exif->fPixelYDimension          = (uint32)val;
            if (meta.getExifTagLong("Exif.Photo.FocalPlaneResolutionUnit", val))       exif->fFocalPlaneResolutionUnit = (uint32)val;
            if (meta.getExifTagLong("Exif.GPSInfo.GPSVersionID", val))                 exif->fGPSVersionID             = (uint32)val;
            if (meta.getExifTagLong("Exif.GPSInfo.GPSAltitudeRef", val))               exif->fGPSAltitudeRef           = (uint32)val;
            if (meta.getExifTagLong("Exif.GPSInfo.GPSDifferential", val))              exif->fGPSDifferential          = (uint32)val;

            // Markernote backup.

            QByteArray mkrnts = meta.getExifTagData("Exif.Photo.MakerNote");
            if (!mkrnts.isEmpty())
            {
                kDebug(51000) << "DNGWriter: Backup Makernote (" << mkrnts.size() << " bytes)" ;

                dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);
                dng_memory_stream stream(memalloc);
                stream.Put(mkrnts.data(), mkrnts.size());
                AutoPtr<dng_memory_block> block(host.Allocate(mkrnts.size()));
                stream.SetReadPosition(0);
                stream.Get(block->Buffer(), mkrnts.size());
                negative->SetMakerNote(block);
                negative->SetMakerNoteSafety(true);
            }
        }

        if (d->backupOriginalRawFile)
        {
            kDebug(51000) << "DNGWriter: Backup Original RAW file (" << inputInfo.size() << " bytes)" ;

            // Compress Raw file data to Zip archive.

            QTemporaryFile zipFile;
            if (!zipFile.open())
            {
                kDebug(51000) << "DNGWriter: Cannot open temporary file to write Zip Raw file. Aborted..." ;
                return -1;
            }
            KZip zipArchive(zipFile.fileName());
            zipArchive.open(QIODevice::WriteOnly);
            zipArchive.setCompression(KZip::DeflateCompression);
            zipArchive.addLocalFile(inputFile(), inputFile());
            zipArchive.close();

            // Load Zip Archive in a byte array

            QFileInfo zipFileInfo(zipFile.fileName());
            QByteArray zipRawFileData;
            zipRawFileData.resize(zipFileInfo.size());
            QDataStream dataStream(&zipFile);
            dataStream.readRawData(zipRawFileData.data(), zipRawFileData.size());
            kDebug(51000) << "DNGWriter: Zipped RAW file size " << zipRawFileData.size() << " bytes" ;

            // Pass byte array to DNG sdk and compute MD5 fingerprint.

            dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);
            dng_memory_stream stream(memalloc);
            stream.Put(zipRawFileData.data(), zipRawFileData.size());
            AutoPtr<dng_memory_block> block(host.Allocate(zipRawFileData.size()));
            stream.SetReadPosition(0);
            stream.Get(block->Buffer(), zipRawFileData.size());

            dng_md5_printer md5;
            md5.Process(block->Buffer(), block->LogicalSize());
            negative->SetOriginalRawFileData(block);
            negative->SetOriginalRawFileDigest(md5.Result());
            negative->ValidateOriginalRawFileDigest();

            zipFile.remove();
        }

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: Build DNG Negative" ;

        // Assign Raw image data.
        negative->SetStage1Image(image);

        // Compute linearized and range mapped image
        negative->BuildStage2Image(host);

        // Compute demosaiced image (used by preview and thumbnail)
        negative->BuildStage3Image(host);

        negative->SynchronizeMetadata();
        negative->RebuildIPTC();

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        dng_preview_list previewList;

// NOTE: something is wrong with Qt < 4.4.0 to import TIFF data as stream in QImage.
#if QT_VERSION >= 0x40400

        if (d->previewMode != DNGWriter::NONE)
        {
            kDebug(51000) << "DNGWriter: DNG preview image creation" ;

            // Construct a preview image as TIFF format.
            AutoPtr<dng_image> tiffImage;
            dng_render tiff_render(host, *negative);
            tiff_render.SetFinalSpace(dng_space_sRGB::Get());
            tiff_render.SetFinalPixelType(ttByte);
            tiff_render.SetMaximumSize(d->previewMode == MEDIUM ? 1280 : width);
            tiffImage.Reset(tiff_render.Render());

            dng_image_writer tiff_writer;
            AutoPtr<dng_memory_stream> dms(new dng_memory_stream(gDefaultDNGMemoryAllocator));

            tiff_writer.WriteTIFF(host, *dms, *tiffImage.Get(), piRGB,
                                  ccUncompressed, negative.Get(), &tiff_render.FinalSpace());

            // Write TIFF preview image data to a temp JPEG file
            std::vector<char> tiff_mem_buffer(dms->Length());
            dms->SetReadPosition(0);
            dms->Get(&tiff_mem_buffer.front(), tiff_mem_buffer.size());
            dms.Reset();

            QImage pre_image;
            if (!pre_image.loadFromData((uchar*)&tiff_mem_buffer.front(), tiff_mem_buffer.size(), "TIFF"))
            {
                kDebug(51000) << "DNGWriter: Cannot load TIFF preview data in memory. Aborted..." ;
                return -1;
            }

            QTemporaryFile previewFile;
            if (!previewFile.open())
            {
                kDebug(51000) << "DNGWriter: Cannot open temporary file to write JPEG preview. Aborted..." ;
                return -1;
            }

            if (!pre_image.save(previewFile.fileName(), "JPEG", 90))
            {
                kDebug(51000) << "DNGWriter: Cannot save file to write JPEG preview. Aborted..." ;
                return -1;
            }

            // Load JPEG preview file data in DNG preview container.
            AutoPtr<dng_jpeg_preview> jpeg_preview;
            jpeg_preview.Reset(new dng_jpeg_preview);
            jpeg_preview->fPhotometricInterpretation = piYCbCr;
            jpeg_preview->fPreviewSize.v             = pre_image.height();
            jpeg_preview->fPreviewSize.h             = pre_image.width();
            jpeg_preview->fCompressedData.Reset(host.Allocate(previewFile.size()));

            QDataStream previewStream( &previewFile );
            previewStream.readRawData(jpeg_preview->fCompressedData->Buffer_char(), previewFile.size());

            AutoPtr<dng_preview> pp( dynamic_cast<dng_preview*>(jpeg_preview.Release()) );
            previewList.Append(pp);

            previewFile.remove();
        }

#endif /* QT_VERSION >= 0x40400 */

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: DNG thumbnail creation" ;

        dng_image_preview thumbnail;
        dng_render thumbnail_render(host, *negative);
        thumbnail_render.SetFinalSpace(dng_space_sRGB::Get());
        thumbnail_render.SetFinalPixelType(ttByte);
        thumbnail_render.SetMaximumSize(256);
        thumbnail.fImage.Reset(thumbnail_render.Render());

        if (d->cancel) return -2;

        // -----------------------------------------------------------------------------------------

        kDebug(51000) << "DNGWriter: Creating DNG file " << outputInfo.fileName() ;

        dng_image_writer writer;
        dng_file_stream filestream(QFile::encodeName(dngFilePath), true);

        writer.WriteDNG(host, filestream, *negative.Get(), thumbnail,
                        d->jpegLossLessCompression ? ccJPEG : ccUncompressed,
                        &previewList);

        // -----------------------------------------------------------------------------------------
        // Metadata makernote cleanup using Exiv2. See B.K.O #204437.

        kDebug(51000) << "DNGWriter: cleanup makernotes using Exiv2" ;

        if (meta.load(dngFilePath))
        {
            meta.setWriteRawFiles(true);
            meta.removeExifTag("Exif.OlympusIp.BlackLevel", false);
            meta.applyChanges();
        }

        // update modification time if desired
        if (d->updateFileDate)
        {
            QDateTime date = meta.getImageDateTime();

            kDebug(51000) << "Setting modification date from meta data: "
                          << date.toString();

            // don't touch access time
            struct stat st;
            stat(QFile::encodeName(dngFilePath), &st);

            struct utimbuf ut;
            ut.actime = st.st_atime;
            ut.modtime = date.toTime_t();
            utime(QFile::encodeName(dngFilePath), &ut);
        }
    }

    catch (const dng_exception &exception)
    {
        int ret = exception.ErrorCode();
        kDebug(51000) << "DNGWriter: DNG SDK exception code (" << ret << ")" ;
        return ret;
    }

    catch (...)
    {
        kDebug(51000) << "DNGWriter: DNG SDK exception code unknow" ;
        return dng_error_unknown;
    }

    kDebug(51000) << "DNGWriter: DNG conversion complete..." ;

    return dng_error_none;
}

}  // namespace DNGIface
