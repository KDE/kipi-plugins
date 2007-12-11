/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : GPS file list view item.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSLISTVIEWITEM_H
#define GPSLISTVIEWITEM_H

// Qt includes.

#include <qdatetime.h>
#include <qstring.h>

// KDE includes.

#include <kurl.h>
#include <klistview.h>

// Local includes.

#include "gpsdatacontainer.h"

class QPainter;
class QColorGroup;
class QPixmap;

namespace KIPIGPSSyncPlugin
{
class GPSListViewItemPriv;

class GPSListViewItem : public KListViewItem
{

public:

    GPSListViewItem(KListView *view, QListViewItem *after, const KURL& url);
    ~GPSListViewItem();

    void setGPSInfo(GPSDataContainer gpsData, bool dirty=true, bool addedManually=false);
    GPSDataContainer GPSInfo();
    void eraseGPSInfo();

    void setDateTime(QDateTime date);
    QDateTime dateTime();

    void setEnabled(bool e);
    bool isEnabled();
    bool isDirty();
    bool isInterpolated();
    bool hasGPSInfo();
    bool isReadOnly();

    KURL url();
    void writeGPSInfoToFile();

protected:

    void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

private: 

    GPSListViewItemPriv *d;
};

} // NameSpace KIPIGPSSyncPlugin

#endif /* GPSLISTVIEWITEM_H */
