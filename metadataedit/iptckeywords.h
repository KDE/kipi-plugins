/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-15
 * Description : IPTC keywords settings page.
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

#ifndef IPTC_KEYWORDS_H
#define IPTC_KEYWORDS_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class IPTCKeywordsPriv;

class IPTCKeywords : public QWidget
{
    Q_OBJECT
    
public:

    IPTCKeywords(QWidget* parent, QByteArray& iptcData);
    ~IPTCKeywords();

    void applyMetadata(QByteArray& iptcData);

private:

    void readMetadata(QByteArray& iptcData);

private slots:

    void slotKeywordSelectionChanged();
    void slotAddKeyword();
    void slotDelKeyword();

private:

    IPTCKeywordsPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_KEYWORDS_H 
