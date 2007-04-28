/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-18
 * Description : EXIF adjustments settings page.
 * 
 * Copyright 2006 by Gilles Caulier <caulier dot gilles at kdemail dot net>
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

#ifndef EXIF_ADJUST_H
#define EXIF_ADJUST_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class EXIFAdjustPriv;

class EXIFAdjust : public QWidget
{
    Q_OBJECT
    
public:

    EXIFAdjust(QWidget* parent);
    ~EXIFAdjust();

    void applyMetadata(QByteArray& exifData);
    void readMetadata(QByteArray& exifData);

signals:

    void signalModified();

private:

    EXIFAdjustPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // EXIF_ADJUST_H 
