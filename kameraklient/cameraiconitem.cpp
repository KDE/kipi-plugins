/* ============================================================
 * File  : cameraiconitem.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-23
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

// Qt
#include <qstring.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpalette.h>
// Local
#include "gpfileiteminfo.h"
#include "cameraiconview.h"
#include "cameraiconitem.h"

namespace KIPIKameraKlientPlugin
{

const char* CameraIconItem::new_xpm[] = {
"13 13 8 1",
"       c None",
".      c #232300",
"+      c #F6F611",
"@      c #000000",
"#      c #DBDA4D",
"$      c #FFFF00",
"%      c #AAA538",
"&      c #E8E540",
"      .      ",
"  .  .+.  .  ",
" @#@ .$. .#. ",
"  @$@#$#@$.  ",
"   @$%&%$@   ",
" ..#%&&&%#.. ",
".+$$&&&&&$$+@",
" ..#%&&&%#@@ ",
"   @$%&%$@   ",
"  .$@#$#@$.  ",
" @#. @$@ @#. ",
"  .  @+@  .  ",
"      @      "};


QPixmap* CameraIconItem::newEmblem = 0;

CameraIconItem::CameraIconItem(ThumbView* parent, const GPFileItemInfo* fileInfo, const QPixmap& pixmap)
    : ThumbItem(parent, fileInfo->name, pixmap), fileInfo_(fileInfo) {
    pixWidth_  = pixmap.width();
    pixHeight_ = pixmap.height();
    loadNewEmblem();
}

CameraIconItem::~CameraIconItem() {
    if(newEmblem) {
        delete newEmblem;
        newEmblem = 0;
    }
}

const GPFileItemInfo* CameraIconItem::fileInfo() {
    return fileInfo_;    
}

void CameraIconItem::loadNewEmblem() {
    if (!newEmblem) {
        newEmblem = new QPixmap(new_xpm);
    }
}

void CameraIconItem::setPixmap(const QImage& thumb) {
    int size = pixmap()->width();
    pixWidth_  = thumb.width();
    pixHeight_ = thumb.height();
    QPainter painter;
    painter.begin(pixmap());
    painter.fillRect(0, 0, size, size, QBrush(iconView()->colorGroup().base()));
    painter.drawImage((size-thumb.width())/2, (size-thumb.height())/2, thumb);
    painter.end();
    repaint();
}

}  // NameSpace KIPIKameraKlientPlugin

