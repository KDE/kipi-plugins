/* ============================================================
 * File  : cameraiconview.cpp
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
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qpalette.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qvaluevector.h>
// KDE
#include <klocale.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <ktrader.h>
#include <kservice.h>
// Local
#include "gpfileiteminfo.h"
#include "gpfileiteminfodlg.h"
#include "cameraiconitem.h"
#include "cameraiconview.h"
#include "cameradragobject.h"
#include "cameraui.h"

namespace KIPIKameraKlientPlugin
{

const int MAXICONITEMS = 307;
const int THUMBSIZE = 120;

class CameraIconViewPrivate {

public:
    QPixmap imagePix;
    QPixmap audioPix;
    QPixmap videoPix;
    QPixmap unknownPix;
};

CameraIconView::CameraIconView(QWidget *parent) : ThumbView(parent) {
    d = new CameraIconViewPrivate;
}

CameraIconView::~CameraIconView() {
    delete d;    
}

void CameraIconView::setThumbnailSize() {
    int w = THUMBSIZE;
    int h = THUMBSIZE;
    QString iconfile = locate("data", "/icons/documents.png");
    QImage image(iconfile);
    double scale = double(w-10) / double(image.width());
    image = image.smoothScale(w-10, h-10, QImage::ScaleMin);
    QPixmap pix(w, h);
    pix.fill(colorGroup().base());
    QPainter p(&pix);
    p.fillRect(0, 0, w, h, QBrush(colorGroup().base()));
    if(!image.isNull()) {
        p.drawImage((w-image.width())/2, (h-image.height())/2, image);
    }
    p.end();
    d->imagePix = pix;
    createPixmap(d->imagePix, "icons/pictures.png", scale);
    d->audioPix = pix;
    createPixmap(d->audioPix, "icons/sound.png", scale);
    d->videoPix = pix;
    createPixmap(d->videoPix, "icons/multimedia.png", scale);
    d->unknownPix = pix;
    createPixmap(d->unknownPix, "icons/documents.png", scale);
}

void CameraIconView::createPixmap(QPixmap& pix, const QString& icon, double scale) {
    QString iconfile = locate("data", icon);
    QImage mimeImg(iconfile);
    mimeImg = mimeImg.smoothScale((int) (mimeImg.width()*scale), (int) (mimeImg.height()*scale), QImage::ScaleMin);
    int w = THUMBSIZE;
    int h = THUMBSIZE;
    QPainter p(&pix);
    if (!mimeImg.isNull()) {
        p.drawImage((w-mimeImg.width())/2, (h-mimeImg.height())/2, mimeImg);
    }
    p.end();
}

CameraIconItem* CameraIconView::addItem(const GPFileItemInfo* fileInfo) {
    QPixmap& pix = d->unknownPix;
    if(fileInfo->mime.contains("image")) {
        pix = d->imagePix;
    } else if(fileInfo->mime.contains("audio")) {
        pix = d->audioPix;
    } else if(fileInfo->mime.contains("video")) {
        pix = d->videoPix;
    } else {
        pix = d->unknownPix;
    } 
    CameraIconItem *iconItem = new CameraIconItem(this, fileInfo, pix);
    return iconItem;
}

void CameraIconView::clear() {
    ThumbView::clear();
    emit signalCleared();
}

void CameraIconView::setThumbnail(CameraIconItem* iconItem, const QImage& thumbnail) {
    if (!iconItem) {
		return;
    }
    iconItem->setPixmap(thumbnail);
}

void CameraIconView::markDownloaded(CameraIconItem* iconItem) {
    if (!iconItem) {
		return;
    }
    GPFileItemInfo *fileInfo = const_cast<GPFileItemInfo*>(iconItem->fileInfo());
    fileInfo->downloaded = 1;
    iconItem->repaint();
}

void CameraIconView::startDrag() {
    int count = 0;
    for (ThumbItem *it=firstItem(); it; it=it->nextItem()) {
        if (it->isSelected()) {
            count++;
		}
    }
    if (count == 0) {
		return;
    }
    CameraDragObject* drag = new CameraDragObject(CameraUI::getInstance()->cameraType(), this);
    drag->setPixmap(SmallIcon("image"));
    drag->dragCopy();
}

}  // NameSpace KIPIKameraKlientPlugin
