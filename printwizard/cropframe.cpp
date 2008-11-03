/***************************************************************************
                          cropframe.cpp  -  description
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

// C Ansi includes.
  
extern "C"
{
#include <math.h>
#include <stdio.h>
}

// Qt includes.
#include <QMatrix>
#include <qpainter.h>
#include <qimage.h>
#include <QMouseEvent>

// Local includes.

#include "cropframe.h"
#include "utils.h"

namespace KIPIPrintWizardPlugin
{

CropFrame::CropFrame(QWidget *parent=0)
         : QWidget(parent)
{
  m_mouseDown = false;
}


CropFrame::~CropFrame()
{
   delete m_pixmap;
}

// FIXME:  This method is doing way too much.  The cropFrame initialization
// should be a TPhoto method, and should not require the scaling of
// pixmaps to get the desired effect, which are too slow.

void CropFrame::init(TPhoto *photo, int width, int height, bool autoRotate, bool paint)
{
  m_photo = photo;
  QImage scaledImg = m_photo->thumbnail().toImage();

  // has the cropRegion been set yet?
  bool resetCropRegion = (m_photo->cropRegion == QRect(-1, -1, -1, -1));
  if (resetCropRegion) 
  {
    // first, let's see if we should rotate
    if (autoRotate) {
      if (m_photo->rotation == 0 && ((width > height &&
          m_photo->thumbnail().height() > m_photo->thumbnail().width()) ||
          (height > width &&
          m_photo->thumbnail().width() > m_photo->thumbnail().height())) )
      {
        // rotate
        m_photo->rotation = 90;
      }
    }
  }
  else
  {
    // does the crop region need updating (but the image shouldn't be rotated)?
    resetCropRegion = (m_photo->cropRegion == QRect(-2, -2, -2, -2));
  }
  
  // rotate
  QMatrix matrix;
  matrix.rotate(m_photo->rotation);
  scaledImg = scaledImg.transformed(matrix);

  scaledImg = scaledImg.scaled(this->width(), this->height(), Qt::KeepAspectRatio); 
  //TODO check for cropping Qt::KeepAspectRatioByExpanding);

  m_pixmap = new QPixmap();
  m_pixmap->fromImage(scaledImg);
  m_pixmapX = (this->width() / 2) - (m_pixmap->width() / 2);
  m_pixmapY = (this->height() / 2) - (m_pixmap->height() / 2);

  m_color = Qt::red;
  // size the rectangle based on the minimum image dimension
  int w = m_pixmap->width();
  int h = m_pixmap->height();;
  if (w < h)
  {
    h = NINT((double)w * ((double)height / (double)width));
    if (h > m_pixmap->height())
    {
      h = m_pixmap->height();
      w = NINT((double)h * ((double)width / (double)height));
    }
  }
  else
  {
    w = NINT((double)h * ((double)width / (double)height));
    if (w > m_pixmap->width())
    {
      w = m_pixmap->width();
      h = NINT((double)w * ((double)height / (double)width));
    }
  }

  if (resetCropRegion)
  {
    m_cropRegion.setRect((this->width() / 2) - (w / 2), (this->height() / 2) - (h / 2), w, h);

    m_photo->cropRegion = _screenToPhotoRect(m_cropRegion);
  }
  else
    m_cropRegion = _photoToScreenRect(m_photo->cropRegion);

  if (paint)  
    repaint(m_cropRegion);
}

QRect CropFrame::_screenToPhotoRect(QRect r)
{
  // r is given in screen coordinates, and we want to convert that
  // to photo coordinates
  double xRatio = 0.0;
  double yRatio = 0.0;

  // flip the photo dimensions if rotated
  int photoW;
  int photoH;

  if (m_photo->rotation == 0 || m_photo->rotation == 180)
  {
    photoW = m_photo->width();
    photoH = m_photo->height();
  }
  else
  {
    photoW = m_photo->height();
    photoH = m_photo->width();
  }
  if (m_pixmap->width() > 0)
    xRatio = (double) photoW / (double) m_pixmap->width();

  if (m_pixmap->height() > 0)
    yRatio = (double) photoH / (double) m_pixmap->height();

  
  int x1 = NINT((r.left() - m_pixmapX) * xRatio);
  int y1 = NINT((r.top()  - m_pixmapY) * yRatio);

  int w  = NINT(r.width() * xRatio);
  int h  = NINT(r.height() * yRatio);

  QRect result;
  result.setRect(x1, y1, w, h);
  return result;
}

QRect CropFrame::_photoToScreenRect(QRect r)
{
  // r is given in photo coordinates, and we want to convert that
  // to screen coordinates
  double xRatio = 0.0;
  double yRatio = 0.0;

  // flip the photo dimensions if rotated
  int photoW;
  int photoH;
  
  if (m_photo->rotation == 0 || m_photo->rotation == 180)
  {
    photoW = m_photo->width();
    photoH = m_photo->height();
  }
  else
  {
    photoW = m_photo->height();
    photoH = m_photo->width();
  }
  
  if (m_photo->width() > 0)
    xRatio = (double) m_pixmap->width() / (double) photoW;

  if (m_photo->height() > 0)
    yRatio = (double)m_pixmap->height() / (double)photoH;


  int x1 = NINT(r.left() * xRatio + m_pixmapX);
  int y1 = NINT(r.top()  * yRatio + m_pixmapY);

  int w  = NINT(r.width() * xRatio);
  int h  = NINT(r.height() * yRatio);

  QRect result;
  result.setRect(x1, y1, w, h);
  return result;
}

void CropFrame::paintEvent (QPaintEvent *)
{

  QPixmap bmp(this->width(), this->height());
  QPainter p;
  p.begin(&bmp);

  p.eraseRect(0, 0, this->width(), this->height());

  // draw the background pixmap
  p.drawPixmap(m_pixmapX, m_pixmapY, *m_pixmap);

  // draw the rectangle
  p.setPen(QPen(m_color, 2));
  p.drawRect(m_cropRegion);
  // draw the crosshairs
  int midX = m_cropRegion.left() + m_cropRegion.width() / 2;
  int midY = m_cropRegion.top()  + m_cropRegion.height() / 2;
  p.drawLine(midX - 10, midY, midX + 10, midY);
  p.drawLine(midX, midY - 10, midX, midY + 10);
  p.end();

  QPainter newp(this);
  newp.drawPixmap(0, 0, bmp);
}

void CropFrame::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    m_mouseDown = true;
    this->mouseMoveEvent(e);
  }

}

void CropFrame::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
    m_mouseDown = false;
}

void CropFrame::mouseMoveEvent(QMouseEvent *e)
{
  if (m_mouseDown)
  {
    // don't let the rectangle float off the image.
    int newW = m_cropRegion.width();
    int newH = m_cropRegion.height();

    int newX = e->x() - (newW / 2);
    newX = MAX(m_pixmapX, newX);
    newX = MIN(m_pixmapX + m_pixmap->width() - newW, newX);

    int newY = e->y() - (newH / 2);
    newY = MAX(m_pixmapY, newY);
    newY = MIN(m_pixmapY + m_pixmap->height() - newH, newY);

    m_cropRegion.setRect(newX, newY, newW, newH);
    m_photo->cropRegion = _screenToPhotoRect(m_cropRegion);
    repaint(m_cropRegion);
  }
}

void CropFrame::keyPressEvent(QKeyEvent *e)
{
  int newX = m_cropRegion.x();
  int newY = m_cropRegion.y();

  switch (e->key()) {
    case Qt::Key_Up : newY--;
                 break;
    case Qt::Key_Down : newY++;
                 break;
    case Qt::Key_Left : newX--;
                 break;
    case Qt::Key_Right : newX++;
                 break;
  }

  // keep inside the pixmap
  int w = m_cropRegion.width();
  int h = m_cropRegion.height();

  newX = MAX(m_pixmapX, newX);
  newX = MIN(m_pixmapX + m_pixmap->width() - w, newX);

  newY = MAX(m_pixmapY, newY);
  newY = MIN(m_pixmapY + m_pixmap->height() - h, newY);

  m_cropRegion.setRect(newX, newY, w, h);
  m_photo->cropRegion = _screenToPhotoRect(m_cropRegion);
  repaint(m_cropRegion);

}

void CropFrame::setColor(QColor c)
{
  m_color = c;
  repaint();
}

QColor CropFrame::color()
{
  return m_color;
}

}  // NameSpace KIPIPrintWizardPlugin

#include "cropframe.moc"
