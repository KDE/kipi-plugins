/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF photo settings page.
 * 
 * Copyright 2006 by Gilles Caulier
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

#ifndef EXIF_PHOTO_H
#define EXIF_PHOTO_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class EXIFPhotoPriv;

class EXIFPhoto : public QWidget
{
    Q_OBJECT
    
public:

    EXIFPhoto(QWidget* parent, QByteArray& exifData);
    ~EXIFPhoto();

    void applyMetadata(QByteArray& exifData);

private:

    void readMetadata(QByteArray& exifData);

private:

    EXIFPhotoPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // EXIF_PHOTO_H 
