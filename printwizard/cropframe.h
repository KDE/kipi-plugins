/***************************************************************************
                          cropframe.h  -  description
                             -------------------
    begin                : Mon Sep 30 2002
    copyright            : (C) 2002 by Todd Shoemaker
    email                : jtshoe11@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CROPFRAME_H
#define CROPFRAME_H

#include <qwidget.h>
#include <qpixmap.h>
#include "tphoto.h"

/**
  *@author Todd Shoemaker
  */

class CropFrame : public QWidget  {
   Q_OBJECT
public:
    CropFrame(QWidget *parent, const char *name);
	  ~CropFrame();
    void init(TPhoto *photo, int width, int height, bool paint = true);
    void setColor(QColor);
    QColor color();
  private:
    TPhoto *m_photo;
    bool m_mouseDown;
    QPixmap *m_pixmap;
    int m_pixmapX;
    int m_pixmapY;

    QColor m_color;

    QRect m_cropRegion;


    QRect _screenToPhotoRect(QRect r);
    QRect _photoToScreenRect(QRect r);
  protected:
    void paintEvent (QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);

};

#endif
