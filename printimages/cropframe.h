/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2002-30-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2002-2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2008 by 2007 Angelo Naselli <anaselli at linux dot it>
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

#ifndef CROPFRAME_H
#define CROPFRAME_H

// Qt includes

#include <QLabel>

// Local includes

#include "tphoto.h"

class QPixmap;

namespace KIPIPrintImagesPlugin
{

class CropFrame : public QWidget
{
  Q_OBJECT

  public:
    CropFrame(QWidget *parent);
    ~CropFrame();
    void init(TPhoto *photo, int width, int height, bool autoRotate, bool paint = true);
    void setColor(QColor);
    QColor color();
    void drawCropRectangle(bool draw=true) {m_drawRec=draw; };

  private:
    TPhoto *m_photo;
    bool m_mouseDown;
    QPixmap *m_pixmap;
    int m_pixmapX;
    int m_pixmapY;

    QColor m_color;

    QRect m_cropRegion;
    bool m_drawRec;


    QRect _screenToPhotoRect(QRect r);
    QRect _photoToScreenRect(QRect r);

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
};

}  // NameSpace KIPIPrintImagesPlugin

#endif // CROPFRAME_H
