/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : IPTC status settings page.
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

#ifndef IPTC_STATUS_H
#define IPTC_STATUS_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class IPTCStatusPriv;

class IPTCStatus : public QWidget
{
    Q_OBJECT
    
public:

    IPTCStatus(QWidget* parent);
    ~IPTCStatus();

    void applyMetadata(QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

signals:

    void signalModified();

private:

    IPTCStatusPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_STATUS_H 
