/* ============================================================
 * File  : clistviewitem.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-25
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef CLISTVIEWITEM_H
#define CLISTVIEWITEM_H

#include <klistview.h>
#include <qstring.h>

class QPixmap;

namespace KIPIRawConverterPlugin
{

class CListViewItem;

struct RawItem {
    QString src;
    QString dest;
    QString directory;
    QString identity;
    CListViewItem *viewItem;
};

class CListViewItem : public KListViewItem
{
public:

    CListViewItem(KListView* view, const QPixmap& pixmap,
                  RawItem *item)
        : KListViewItem(view), rawItem(item) {
         rawItem->viewItem = this;
         setThumbnail(pixmap);
         setText(1, rawItem->src);
         setText(2, rawItem->dest);
    }

    ~CListViewItem() {
    }

    void setThumbnail(const QPixmap& pixmap) {
        setPixmap(0, pixmap);
    }
    
    struct RawItem *rawItem;

};

} // NameSpace KIPIRawConverterPlugin

#endif /* CLISTVIEWITEM_H */
