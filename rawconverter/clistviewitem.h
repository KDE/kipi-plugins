/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-25
 * Description : Raw file list view used into batch converter.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef CLISTVIEWITEM_H
#define CLISTVIEWITEM_H

// Qt includes.

#include <QBrush>
#include <QTreeWidgetItem>
#include <QPainter>
#include <QColorGroup>

class QPixmap;

namespace KIPIRawConverterPlugin
{

class CListViewItem;

struct RawItem 
{
    QString        src;
    QString        dest;
    QString        directory;
    QString        identity;

    CListViewItem *viewItem;
};

class CListViewItem : public QTreeWidgetItem
{

public:

    struct RawItem *rawItem;

public:

    CListViewItem(QTreeWidget *view, const QPixmap& pixmap, RawItem *item)
                : QTreeWidgetItem(view), rawItem(item) 
    {
         rawItem->viewItem = this;
         setIcon(0, pixmap);
         setText(1, rawItem->src);
         setText(2, rawItem->dest);
         setEnabled(true);
    }

    ~CListViewItem(){}

    void setThumbnail(const QPixmap& pixmap) 
    {
        setIcon(0, pixmap);
    }

    void setEnabled(bool d)
    {
        m_enabled = d;
        setForeground(0, QBrush(Qt::gray));
        setForeground(1, QBrush(Qt::gray));
        setForeground(2, QBrush(Qt::gray));
    }

    bool isEnabled()
    {
        return m_enabled;
    }

private: 

    bool m_enabled;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* CLISTVIEWITEM_H */
