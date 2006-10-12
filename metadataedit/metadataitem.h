/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-11
 * Description : Metadata file list view item.
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

#ifndef METADATAITEM_H
#define METADATAITEM_H

// Qt includes.

#include <qstring.h>
#include <qcstring.h>

// KDE includes.

#include <kurl.h>
#include <klistview.h>

class QPainter;
class QColorGroup;
class QPixmap;

class KFileItem;

namespace KIPIMetadataEditPlugin
{
class MetadataItemPriv;

class MetadataItem : public QObject, public KListViewItem
{

Q_OBJECT

public:

    MetadataItem(KListView *view, QListViewItem *after, const KURL& url);
    ~MetadataItem();

    void setExif(const QByteArray& exifData, bool dirty);
    void setIptc(const QByteArray& iptcData, bool dirty);
    QByteArray getExif();
    QByteArray getIptc();
    void eraseExif();
    void eraseIptc();

    void setEnabled(bool e);
    bool isEnabled();
    bool isDirty();
    bool hasExif();
    bool hasIptc();
    bool isReadOnly();

    KURL getUrl();
    void writeMetadataToFile();
        
protected:

    void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

private slots:

    void slotGotThumbnail(const KFileItem*, const QPixmap&);

private: 

    MetadataItemPriv *d;
};

} // NameSpace KIPIMetadataEditPlugin

#endif /* METADATAITEM_H */
