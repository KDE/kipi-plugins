/* ============================================================
 * File  : cameraiconitem.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-23
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

#ifndef CAMERAICONITEM_H
#define CAMERAICONITEM_H

#include "thumbitem.h"

class QString;
class QPixmap;
class QImage;
class QPainter;
class QColorGroup;

namespace KIPIKameraKlientPlugin
{

class GPFileItemInfo;
class ThumbView;

class CameraIconItem : public ThumbItem {
    friend class CameraIconView;
    
public:
    CameraIconItem(ThumbView* parent, const GPFileItemInfo* fileInfo, const QPixmap& pixmap);
    ~CameraIconItem();
    const GPFileItemInfo* fileInfo();
    void setPixmap(const QImage& thumb);
 
private:
    void loadNewEmblem();
    const GPFileItemInfo* fileInfo_;
    int pixWidth_, pixHeight_;
    static QPixmap* newEmblem;
    static const char* new_xpm[];
};

}  // NameSpace KIPIKameraKlientPlugin

#endif 

