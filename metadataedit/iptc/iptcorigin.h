/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-13
 * Description : IPTC origin settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IPTC_ORIGIN_H
#define IPTC_ORIGIN_H

// Qt includes

#include <QDateTime>
#include <QWidget>
#include <QByteArray>

namespace KIPIMetadataEditPlugin
{

class IPTCOriginPriv;

class IPTCOrigin : public QWidget
{
    Q_OBJECT

public:

    IPTCOrigin(QWidget* parent);
    ~IPTCOrigin();

    void applyMetadata(QByteArray& exifData, QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

    bool syncHOSTDateIsChecked();
    bool syncEXIFDateIsChecked();

    void setCheckedSyncHOSTDate(bool c);
    void setCheckedSyncEXIFDate(bool c);

    QDateTime getIPTCCreationDate();

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotSetTodayCreated();
    void slotSetTodayDigitalized();

private:

    IPTCOriginPriv* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_ORIGIN_H 
