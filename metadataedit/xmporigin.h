/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-24
 * Description : XMP origin settings page.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef XMP_ORIGIN_H
#define XMP_ORIGIN_H

// Qt includes

#include <QDateTime>
#include <QWidget>
#include <QByteArray>

namespace KIPIMetadataEditPlugin
{

class XMPOriginPriv;

class XMPOrigin : public QWidget
{
    Q_OBJECT

public:

    XMPOrigin(QWidget* parent);
    ~XMPOrigin();

    void applyMetadata(QByteArray& exifData, QByteArray& xmpData);
    void readMetadata(QByteArray& xmpData);

    bool syncHOSTDateIsChecked();
    bool syncEXIFDateIsChecked();

    void setCheckedSyncHOSTDate(bool c);
    void setCheckedSyncEXIFDate(bool c);

    QDateTime getXMPCreationDate();

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotSetTodayCreated();
    void slotSetTodayDigitalized();

private:

    XMPOriginPriv* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // XMP_ORIGIN_H 
