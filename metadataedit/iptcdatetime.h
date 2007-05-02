/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2006-10-13
 * Description : IPTC date and time settings page.
 * 
 * Copyright 2006 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IPTC_DATETIME_H
#define IPTC_DATETIME_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class IPTCDateTimePriv;

class IPTCDateTime : public QWidget
{
    Q_OBJECT
    
public:

    IPTCDateTime(QWidget* parent);
    ~IPTCDateTime();

    void applyMetadata(QByteArray& exifData, QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

    bool syncHOSTDateIsChecked();
    bool syncEXIFDateIsChecked();

    void setCheckedSyncHOSTDate(bool c);
    void setCheckedSyncEXIFDate(bool c);

    QDateTime getIPTCCreationDate();

signals:

    void signalModified();

private:

    IPTCDateTimePriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_DATETIME_H 
