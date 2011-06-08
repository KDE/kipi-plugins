// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2008 Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#include "imagegenerationfunctor.h"

// Qt
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>

// KDE
#include <klocale.h>
#include <kdebug.h>

// KIPI
#include <libkipi/interface.h>

// LibKExiv2 includes
#include <libkexiv2/kexiv2.h>
#include <libkexiv2/version.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>
#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local
#include "galleryinfo.h"
#include "generator.h"
#include "imageelement.h"

using namespace KDcrawIface;

namespace KIPIHTMLExport {


/**
 * Genearate a square thumbnail from @fullImage of @size x @size pixels
 */
static QImage generateSquareThumbnail(const QImage& fullImage, int size) {
    QImage image = fullImage.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    if (image.width() != size || image.height() != size) {
        int sx=0, sy=0;
        if (image.width() > size) {
            sx=(image.width() - size) / 2;
        } else {
            sy=(image.height() - size) / 2;
        }
        image = image.copy(sx, sy, size, size);
    }

    return image;
}


ImageGenerationFunctor::ImageGenerationFunctor(Generator* generator, GalleryInfo* info, const QString& destDir)
: mGenerator(generator)
, mInfo(info)
, mDestDir(destDir)
{}

void ImageGenerationFunctor::operator()(ImageElement& element)
{
    // Load image
    QString path = element.mPath;
    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        emitWarning(i18n("Could not read image '%1'", path));
        return;
    }

    QImage originalImage;

    // Check if RAW file.
    QFileInfo fi(path);
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    QString imageFormat;
    QByteArray imageData;

    if (rawFilesExt.toUpper().contains(fi.suffix().toUpper()) && !mInfo->useOriginalImageAsFullImage()) {
        if (!KDcrawIface::KDcraw::loadDcrawPreview(originalImage, path)) {
            emitWarning(i18n("Error loading RAW image '%1'", path));
            return;
        }
    }
    else {
        imageFormat = QImageReader::imageFormat(&imageFile);
        if (imageFormat.isEmpty()) {
            emitWarning(i18n("Format of image '%1' is unknown", path));
            return;
        }
        imageFile.close();
        imageFile.open(QIODevice::ReadOnly);

        imageData = imageFile.readAll();

        if (!originalImage.loadFromData(imageData) ) {
            emitWarning(i18n("Error loading image '%1'", path));
            return;
        }
    }

    // Process images
    QImage fullImage = originalImage;
    if (!mInfo->useOriginalImageAsFullImage()) {
        if (mInfo->fullResize()) {
            int size = mInfo->fullSize();
            fullImage = fullImage.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        if (element.mAngle != 0) {
            QMatrix matrix;
            matrix.rotate(element.mAngle);
            fullImage = fullImage.transformed(matrix);
        }
    }

    QImage thumbnail = generateSquareThumbnail(fullImage, mInfo->thumbnailSize());

    // Save images
    QString baseFileName = Generator::webifyFileName(element.mTitle);
    baseFileName = mUniqueNameHelper.makeNameUnique(baseFileName);

    // Save full
    QString fullFileName;
    if (mInfo->useOriginalImageAsFullImage()) {
        fullFileName = baseFileName + '.' + imageFormat.toLower();
        if (!writeDataToFile(imageData, mDestDir + '/' + fullFileName)) {
            return;
        }

    } else {
        fullFileName = baseFileName + '.' + mInfo->fullFormatString().toLower();
        QString destPath = mDestDir + '/' + fullFileName;
        if (!fullImage.save(destPath, mInfo->fullFormatString().toAscii(), mInfo->fullQuality())) {
            emitWarning(i18n("Could not save image '%1' to '%2'", path, destPath));
            return;
        }
    }
    element.mFullFileName = fullFileName;
    element.mFullSize = fullImage.size();

    // Save original
    if (mInfo->copyOriginalImage()) {
        QString originalFileName = "original_" + fullFileName;
        if (!writeDataToFile(imageData, mDestDir + '/' + originalFileName)) {
            return;
        }
        element.mOriginalFileName = originalFileName;
        element.mOriginalSize = originalImage.size();
    }

    // Save thumbnail
    QString thumbnailFileName = "thumb_" + baseFileName + '.' + mInfo->thumbnailFormatString().toLower();
    QString destPath = mDestDir + '/' + thumbnailFileName;
    if (!thumbnail.save(destPath, mInfo->thumbnailFormatString().toAscii(), mInfo->thumbnailQuality())) {
        mGenerator->logWarningRequested(i18n("Could not save thumbnail for image '%1' to '%2'", path, destPath));
        return;
    }
    element.mThumbnailFileName = thumbnailFileName;
    element.mThumbnailSize = thumbnail.size();

    element.mValid = true;

        // Read Exif Metadata
        QString unavailable(i18n("unavailable"));
        KExiv2Iface::KExiv2 exiv2Iface;
        exiv2Iface.load(path);
        if (exiv2Iface.hasExif() || exiv2Iface.hasXmp())
        {
            // Try to use libkexiv2 to get image info

            element.mExifImageMake = exiv2Iface.getExifTagString("Exif.Image.Make");
            if (element.mExifImageMake.isEmpty())
            {
                element.mExifImageMake = exiv2Iface.getXmpTagString("Xmp.tiff.Make");
            }

            element.mExifImageModel = exiv2Iface.getExifTagString("Exif.Image.Model");
            if (element.mExifImageModel.isEmpty())
            {
                element.mExifImageModel = exiv2Iface.getXmpTagString("Xmp.tiff.Model");
            }

            element.mExifImageOrientation = exiv2Iface.getExifTagString("Exif.Image.Orientation");
            if (element.mExifImageOrientation.isEmpty())
            {
                element.mExifImageOrientation = exiv2Iface.getXmpTagString("Xmp.tiff.Orientation");
            }

            element.mExifImageXResolution = exiv2Iface.getExifTagString("Exif.Image.XResolution");
            if (element.mExifImageXResolution.isEmpty())
            {
                element.mExifImageXResolution = exiv2Iface.getXmpTagString("Xmp.tiff.XResolution");
            }

            element.mExifImageYResolution = exiv2Iface.getExifTagString("Exif.Image.YResolution");
            if (element.mExifImageYResolution.isEmpty())
            {
                element.mExifImageYResolution = exiv2Iface.getXmpTagString("Xmp.tiff.YResolution");
            }

            element.mExifImageResolutionUnit = exiv2Iface.getExifTagString("Exif.Image.ResolutionUnit");
            if (element.mExifImageResolutionUnit.isEmpty())
            {
                element.mExifImageResolutionUnit = exiv2Iface.getXmpTagString("Xmp.tiff.ResolutionUnit");
            }

            if (exiv2Iface.getImageDateTime().isValid())
            {
                element.mExifImageDateTime = KGlobal::locale()->formatDateTime(exiv2Iface.getImageDateTime(),
                                                             KLocale::ShortDate, true);
            }

            element.mExifImageYCbCrPositioning = exiv2Iface.getExifTagString("Exif.Image.YCbCrPositioning");
            if (element.mExifImageYCbCrPositioning.isEmpty())
            {
                element.mExifImageYCbCrPositioning = exiv2Iface.getXmpTagString("Xmp.tiff.YCbCrPositioning");
            }

            element.mExifPhotoFNumber = exiv2Iface.getExifTagString("Exif.Photo.FNumber");
            if (element.mExifPhotoFNumber.isEmpty())
            {
                element.mExifPhotoFNumber = exiv2Iface.getXmpTagString("Xmp.exif.FNumber");
            }

            element.mExifPhotoApertureValue = exiv2Iface.getExifTagString("Exif.Photo.ApertureValue");
            if (element.mExifPhotoApertureValue.isEmpty())
            {
                element.mExifPhotoApertureValue = exiv2Iface.getXmpTagString("Xmp.exif.ApertureValue");
            }

            element.mExifPhotoFocalLength = exiv2Iface.getExifTagString("Exif.Photo.FocalLength");
            if (element.mExifPhotoFocalLength.isEmpty())
            {
                element.mExifPhotoFocalLength = exiv2Iface.getXmpTagString("Xmp.exif.FocalLength");
            }

            element.mExifPhotoExposureTime = exiv2Iface.getExifTagString("Exif.Photo.ExposureTime");
            if (element.mExifPhotoExposureTime.isEmpty())
            {
                element.mExifPhotoExposureTime = exiv2Iface.getXmpTagString("Xmp.exif.ExposureTime");
            }

            element.mExifPhotoShutterSpeedValue = exiv2Iface.getExifTagString("Exif.Photo.ShutterSpeedValue");
            if (element.mExifPhotoShutterSpeedValue.isEmpty())
            {
                element.mExifPhotoShutterSpeedValue = exiv2Iface.getXmpTagString("Xmp.exif.ShutterSpeedValue");
            }

            element.mExifPhotoISOSpeedRatings = exiv2Iface.getExifTagString("Exif.Photo.ISOSpeedRatings");
            if (element.mExifPhotoISOSpeedRatings.isEmpty())
            {
                element.mExifPhotoISOSpeedRatings = exiv2Iface.getXmpTagString("Xmp.exif.ISOSpeedRatings");
            }

            element.mExifPhotoExposureProgram = exiv2Iface.getExifTagString("Exif.Photo.ExposureIndex");
            if (element.mExifPhotoExposureProgram.isEmpty())
            {
                element.mExifPhotoExposureProgram = exiv2Iface.getXmpTagString("Xmp.exif.ExposureIndex");
            }

        }
        else
        {
            // Try to use libkdcraw interface to identify image.

            DcrawInfoContainer info;
            KDcraw             dcrawIface;
            dcrawIface.rawFileIdentify(info, path);

            if (info.isDecodable)
            {
                if (!info.make.isEmpty())
                    element.mExifImageMake = info.make;

                if (!info.model.isEmpty())
                    element.mExifImageModel = info.model;

                if (info.dateTime.isValid())
                    element.mExifImageDateTime = KGlobal::locale()->formatDateTime(info.dateTime, KLocale::ShortDate, true);

                if (info.aperture != -1.0)
                    element.mExifPhotoApertureValue = QString::number(info.aperture);

                if (info.focalLength != -1.0)
                    element.mExifPhotoFocalLength = QString::number(info.focalLength);

                if (info.exposureTime != -1.0)
                    element.mExifPhotoExposureTime = QString::number(info.exposureTime);

                if (info.sensitivity != -1)
                    element.mExifPhotoISOSpeedRatings = QString::number(info.sensitivity);
            }
        }

           if (element.mExifImageMake.isEmpty()) element.mExifImageMake = unavailable;

           if (element.mExifImageModel.isEmpty()) element.mExifImageModel   = unavailable;

           if (element.mExifImageOrientation.isEmpty()) element.mExifImageOrientation = unavailable;

           if (element.mExifImageXResolution.isEmpty()) element.mExifImageXResolution = unavailable;

           if (element.mExifImageYResolution.isEmpty()) element.mExifImageYResolution = unavailable;

           if (element.mExifImageResolutionUnit.isEmpty()) element.mExifImageResolutionUnit = unavailable;

           if (element.mExifImageDateTime.isEmpty()) element.mExifImageDateTime = unavailable;

           if (element.mExifImageYCbCrPositioning.isEmpty()) element.mExifImageYCbCrPositioning = unavailable;

           if (element.mExifPhotoApertureValue.isEmpty()) element.mExifPhotoApertureValue   = unavailable;

           if (element.mExifPhotoFocalLength.isEmpty()) element.mExifPhotoFocalLength   = unavailable;

           if (element.mExifPhotoFNumber.isEmpty()) element.mExifPhotoFNumber   = unavailable;

           if (element.mExifPhotoExposureTime.isEmpty()) element.mExifPhotoExposureTime = unavailable;

           if (element.mExifPhotoShutterSpeedValue.isEmpty()) element.mExifPhotoShutterSpeedValue = unavailable;

           if (element.mExifPhotoISOSpeedRatings.isEmpty()) element.mExifPhotoISOSpeedRatings   = unavailable;

           if (element.mExifPhotoExposureProgram.isEmpty()) element.mExifPhotoExposureProgram   = unavailable;


}


bool ImageGenerationFunctor::writeDataToFile(const QByteArray& data, const QString& destPath) {
    QFile destFile(destPath);
    if (!destFile.open(QIODevice::WriteOnly)) {
        emitWarning(i18n("Could not open file '%1' for writing", destPath));
        return false;
    }
    if (destFile.write(data) != data.size()) {
        emitWarning(i18n("Could not save image to file '%1'", destPath));
        return false;
    }
    return true;
}


void ImageGenerationFunctor::emitWarning(const QString& message) {
    emit mGenerator->logWarningRequested(message);
}

} // namespace
