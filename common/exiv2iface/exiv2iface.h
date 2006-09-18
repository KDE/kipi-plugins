/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2006-09-15
 * Description : Exiv2 library interface
 *
 * Copyright 2006 by Gilles Caulier
 *
 * NOTE: This class is a simplified version of Digikam::DMetadata
 *       class from digiKam core. Please contact digiKam team 
 *       before to change/fix/improve this implementation.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef EXIV2IFACE_H
#define EXIV2IFACE_H

// QT includes.

#include <qcstring.h>
#include <qstring.h>
#include <qimage.h>
#include <qdatetime.h>

namespace KIPIPlugins
{

class Exiv2IfacePriv;

class Exiv2Iface
{

public:

    enum ImageOrientation
    {
        ORIENTATION_UNSPECIFIED  = 0, 
        ORIENTATION_NORMAL       = 1, 
        ORIENTATION_HFLIP        = 2, 
        ORIENTATION_ROT_180      = 3, 
        ORIENTATION_VFLIP        = 4, 
        ORIENTATION_ROT_90_HFLIP = 5, 
        ORIENTATION_ROT_90       = 6, 
        ORIENTATION_ROT_90_VFLIP = 7, 
        ORIENTATION_ROT_270      = 8
    };

public:

    Exiv2Iface();
    ~Exiv2Iface();
    
    bool load(const QString& filePath);
    bool save(const QString& filePath);

    QByteArray getComments() const;
    QByteArray getExif() const;
    QByteArray getIptc(bool addIrbHeader=false) const;

    void setComments(const QByteArray& data);
    void setExif(const QByteArray& data);
    void setIptc(const QByteArray& data);

    bool setImageProgramId(const QString& program, const QString& version);
    bool setImageDimensions(const QSize& size);
    bool setExifThumbnail(const QImage& thumb);
    bool setExifTagString(const char *exifTagName, const QString& value);
    bool setImagePreview(const QImage& preview);
    bool setImageOrientation(ImageOrientation orientation);

    Exiv2Iface::ImageOrientation getImageOrientation();
    QDateTime getImageDateTime() const;

private:

    Exiv2IfacePriv *d;
};

}  // NameSpace KIPIPlugins

#endif /* EXIV2IFACE_H */
