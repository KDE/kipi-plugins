/* ============================================================
 * Authors     : Gilles Caulier
 * Date        : 2006-10-18
 * Description : EXIF light settings page.
 * 
 * Copyright 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EXIF_LIGHT_H
#define EXIF_LIGHT_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class EXIFLightPriv;

class EXIFLight : public QWidget
{
    Q_OBJECT
    
public:

    EXIFLight(QWidget* parent);
    ~EXIFLight();

    void applyMetadata(QByteArray& exifData);
    void readMetadata(QByteArray& exifData);

signals:

    void signalModified();

private:

    EXIFLightPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // EXIF_LIGHT_H 
