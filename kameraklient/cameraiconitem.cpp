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

