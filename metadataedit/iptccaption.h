/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2006-10-12
 * Description : IPTC caption settings page.
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

#ifndef IPTC_CAPTION_H
#define IPTC_CAPTION_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>

namespace KIPIMetadataEditPlugin
{

class IPTCCaptionPriv;

class IPTCCaption : public QWidget
{
    Q_OBJECT
    
public:

    IPTCCaption(QWidget* parent);
    ~IPTCCaption();

    void applyMetadata(QByteArray& exifData, QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

    bool syncJFIFCommentIsChecked();
    bool syncHOSTCommentIsChecked();
    bool syncEXIFCommentIsChecked();

    void setCheckedSyncJFIFComment(bool c);
    void setCheckedSyncHOSTComment(bool c);
    void setCheckedSyncEXIFComment(bool c);
    
    QString getIPTCCaption();

signals:

    void signalModified();

private:

    IPTCCaptionPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_CAPTION_H 
