/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : EXIF date and time settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EXIF_DATETIME_H
#define EXIF_DATETIME_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>
#include <qdatetime.h>

namespace KIPIMetadataEditPlugin
{

class EXIFDateTimePriv;

class EXIFDateTime : public QWidget
{
    Q_OBJECT
    
public:

    EXIFDateTime(QWidget* parent);
    ~EXIFDateTime();

    void applyMetadata(QByteArray& exifData, QByteArray& iptcData);
    void readMetadata(QByteArray& exifData);

    bool syncHOSTDateIsChecked();
    bool syncIPTCDateIsChecked();

    void setCheckedSyncHOSTDate(bool c);
    void setCheckedSyncIPTCDate(bool c);

    QDateTime getEXIFCreationDate();

signals:

    void signalModified();

private:

    EXIFDateTimePriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // EXIF_DATETIME_H 
