/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2006-10-12
 * Description : EXIF caption settings page.
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

#ifndef EXIF_CAPTION_H
#define EXIF_CAPTION_H

// Qt includes.

#include <qwidget.h>
#include <qcstring.h>
#include <qstring.h>

namespace KIPIMetadataEditPlugin
{

class EXIFCaptionPriv;

class EXIFCaption : public QWidget
{
    Q_OBJECT
    
public:

    EXIFCaption(QWidget* parent);
    ~EXIFCaption();

    void applyMetadata(QByteArray& exifData, QByteArray& iptcData);
    void readMetadata(QByteArray& exifData);

    bool syncJFIFCommentIsChecked();
    bool syncHOSTCommentIsChecked();
    bool syncIPTCCaptionIsChecked();

    void setCheckedSyncJFIFComment(bool c);
    void setCheckedSyncHOSTComment(bool c);
    void setCheckedSyncIPTCCaption(bool c);

    QString getEXIFUserComments();

signals:

    void signalModified();

private:

    EXIFCaptionPriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // EXIF_CAPTION_H 
