/***************************************************************************
                          tphoto.cpp  -  description
                             -------------------
    begin                : Thu Sep 12 2002
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

// Qt includes.
 
#include <qpainter.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qstringlist.h>
#include <qurl.h>
#include <qstrlist.h>

// KDE includes.

#include <kprinter.h>
#include <kdebug.h>

// Local includes.

#include "tphoto.h"
#include "utils.h"

#define IMAGE_FILE_MASK "*"
//"*.jpg;*.jpeg;*.JPG;*.JPEG;*.png;*.PNG"

namespace KIPIPrintWizardPlugin
{

TPhoto::TPhoto(int thumbnailSize)
{
  m_size = 0;
  cropRegion = QRect(-1, -1, -1, -1);
  rotation = 0;
  copies = 1;

  filename = "";

  m_thumbnail = 0;

  this->m_thumbnailSize = thumbnailSize;
}

TPhoto::~TPhoto()
{
  if (m_thumbnail)
    delete m_thumbnail;
  if (m_size)
    delete m_size;
}

void TPhoto::loadCache()
{
  // load the thumbnail and size only once.
  if (m_thumbnail)
    delete m_thumbnail;

  QImage photo;
  photo.load(filename.path()); // PENDING(blackie) handle URL
  m_thumbnail = new QPixmap(QImage( photo.scale(m_thumbnailSize, m_thumbnailSize, QImage::ScaleMin) ));

  if (m_size)
    delete m_size;
  m_size = new QSize(photo.width(), photo.height());
}

QPixmap & TPhoto::thumbnail()
{
  if (m_thumbnail == 0)
    loadCache();
  return *m_thumbnail;
}

QSize & TPhoto::size()  // private
{
  if (m_size == 0)
    loadCache();
  return *m_size;
}

int TPhoto::width()
{
  return size().width();
}

int TPhoto::height()
{
  return size().height();
}

const float FONT_HEIGHT_RATIO = 0.08;

bool paintOnePage(QPainter &p, QPtrList<TPhoto> photos, QPtrList<QRect> layouts,
  int captionType, unsigned int &current, bool useThumbnails)
{
    Q_ASSERT(layouts.count() > 1);

  QRect *srcPage = layouts.at(0);
  QRect *layout = layouts.at(1);

  // scale the page size to best fit the painter
  // size the rectangle based on the minimum image dimension
  int destW = p.window().width();
  int destH = p.window().height();

  int srcW = srcPage->width();
  int srcH = srcPage->height();
  if (destW < destH)
  {
    destH = NINT((double)destW * ((double)srcH / (double)srcW));
    if (destH > p.window().height())
    {
      destH = p.window().height();
      destW = NINT((double)destH * ((double)srcW / (double)srcH));
    }
  }
  else
  {
    destW = NINT((double)destH * ((double)srcW / (double)srcH));
    if (destW > p.window().width())
    {
      destW = p.window().width();
      destH = NINT((double)destW * ((double)srcH / (double)srcW));
    }
  }

  double xRatio = (double)destW / (double)srcPage->width();
  double yRatio = (double)destH / (double)srcPage->height();

  int left = (p.window().width()  - destW) / 2;
  int top  = (p.window().height() - destH) / 2;

  // FIXME: may not want to erase the background page
  p.eraseRect(left, top,
    NINT((double)srcPage->width() * xRatio),
    NINT((double)srcPage->height() * yRatio));

  for(; current < photos.count(); current++)
  {
    TPhoto *photo = photos.at(current);
    // crop
    QImage img;
    if (useThumbnails)
      img = photo->thumbnail().convertToImage();
    else
      img.load(photo->filename.path()); // PENDING(blackie) handle general URL case

    // next, do we rotate?
    if (photo->rotation != 0)
    {
      // rotate
      QWMatrix matrix;
      matrix.rotate(photo->rotation);
      img = img.xForm(matrix);
    }

    if (useThumbnails)
    {
      // scale the crop region to thumbnail coords
      double xRatio = 0.0;
      double yRatio = 0.0;

      if (photo->thumbnail().width() != 0)
        xRatio = (double)photo->thumbnail().width() / (double) photo->width();
      if (photo->thumbnail().height() != 0)
        yRatio = (double)photo->thumbnail().height() / (double) photo->height();

      int x1 = NINT((double)photo->cropRegion.left() * xRatio);
      int y1 = NINT((double)photo->cropRegion.top()  * yRatio);

      int w = NINT((double)photo->cropRegion.width()  * xRatio);
      int h = NINT((double)photo->cropRegion.height() * yRatio);

      img = img.copy(QRect(x1, y1, w, h));
    }
    else
      img = img.copy(photo->cropRegion);

    int x1 = NINT((double)layout->left() * xRatio);
    int y1 = NINT((double)layout->top()  * yRatio);
    int w  = NINT((double)layout->width() * xRatio);
    int h  = NINT((double)layout->height() * yRatio);

    p.drawImage( QRect(x1 + left, y1 + top, w, h), img );

    if (captionType > 0)
    {
      p.save();
      QString caption;
      if (captionType == 1)
      {
        QFileInfo fi(photo->filename.path());
        caption = fi.fileName();
      }
      // draw the text at (0,0), but we will translate and rotate the world
      // before drawing so the text will be in the correct location
      // next, do we rotate?
      int captionW = w-2;
      int captionH = (int)(QMIN(w, h) * FONT_HEIGHT_RATIO);
      if (photo->rotation == 90 || photo->rotation == 270)
      {
        captionW = h;
      }
      p.rotate(photo->rotation);

      int tx = left;
      int ty = top;

      switch(photo->rotation) {
        case 0 : { 
                   tx += x1 + 1;
                   ty += y1 + (h - captionH - 1);
                   break;
                 }
        case 90 : { 
                   tx = top + y1 + 1;
                   ty = -left - x1 - captionH - 1;
                   break;
                 }
        case 180 : { 
                   tx = -left - x1 - w + 1;
                   ty = -top -y1 - (captionH + 1);
                   break;
                 }
        case 270 : { 
                   tx = -top - y1 - h + 1;
                   ty = left + x1 + (w - captionH)- 1;
                   break;
                 }
      }
      p.translate(tx, ty);

      // Now draw the caption
      QFont font;
      font.setStyleHint(QFont::SansSerif);
      font.setPixelSize( (int)(captionH * 0.8) );
      font.setWeight(QFont::Normal);

      p.setFont(font);
      p.setPen(Qt::white);

      QRect r(0, 0, captionW, captionH);
      p.drawText(r, Qt::AlignLeft, caption, -1, &r);
      p.restore();
    } // caption

    // iterate to the next position
    layout = layouts.next();
    if (layout == 0)
    {
      current++;
      break;
    }
  }
  // did we print the last photo?
  return (current < photos.count());
}


// Like above, but outputs to an initialized QImage.  UseThumbnails is
// not an option.
// We have to use QImage for saving to a file, otherwise we would have
// to use a QPixmap, which will have the same bit depth as the display.
// So someone with an 8-bit display would not be able to save 24-bit
// images!
bool paintOnePage(QImage &p, QPtrList<TPhoto> photos, QPtrList<QRect> layouts,
  int captionType, unsigned int &current)
{
    Q_ASSERT(layouts.count() > 1);

  QRect *srcPage = layouts.at(0);
  QRect *layout = layouts.at(1);

  // scale the page size to best fit the painter
  // size the rectangle based on the minimum image dimension
  int destW = p.width();
  int destH = p.height();

  int srcW = srcPage->width();
  int srcH = srcPage->height();
  if (destW < destH)
  {
    destH = NINT((double)destW * ((double)srcH / (double)srcW));
    if (destH > p.height())
    {
      destH = p.height();
      destW = NINT((double)destH * ((double)srcW / (double)srcH));
    }
  }
  else
  {
    destW = NINT((double)destH * ((double)srcW / (double)srcH));
    if (destW > p.width())
    {
      destW = p.width();
      destH = NINT((double)destW * ((double)srcH / (double)srcW));
    }
  }

  double xRatio = (double)destW / (double)srcPage->width();
  double yRatio = (double)destH / (double)srcPage->height();

  int left = (p.width()  - destW) / 2;
  int top  = (p.height() - destH) / 2;


  p.fill(0xffffff);

  for(; current < photos.count(); current++)
  {
    TPhoto *photo = photos.at(current);
    // crop
    QImage img;
    img.load(photo->filename.path()); // PENDING(blackie) handle general URL case

    // next, do we rotate?
    if (photo->rotation != 0)
    {
      // rotate
      QWMatrix matrix;
      matrix.rotate(photo->rotation);
      img = img.xForm(matrix);
    }

    img = img.copy(photo->cropRegion);

    int x1 = NINT((double)layout->left() * xRatio);
    int y1 = NINT((double)layout->top()  * yRatio);
    int w  = NINT((double)layout->width() * xRatio);
    int h  = NINT((double)layout->height() * yRatio);

    // We can use scaleFree because the crop frame should have the proper dimensions.
    img = img.smoothScale(w, h, QImage::ScaleFree);
    // don't have drawimage, so we copy the pixels over manually
    for(int srcY = 0; srcY < img.height(); srcY++)
      for(int srcX = 0; srcX < img.width(); srcX++)
      {
        p.setPixel(x1 + left + srcX, y1 + top + srcY, img.pixel(srcX, srcY));
      }

    if (captionType > 0)
    {
      // Now draw the caption
      QString caption;
      if (captionType == 1)
      {
        QFileInfo fi(photo->filename.path());
        caption = fi.fileName();
      }
      int captionW = w-2;
      int captionH = (int)(QMIN(w, h) * FONT_HEIGHT_RATIO);
      if (photo->rotation == 90 || photo->rotation == 270)
      {
        captionW = h;
      }
      QFont font;
      font.setStyleHint(QFont::SansSerif);
      font.setPixelSize( (int)(captionH * 0.8) );
      font.setWeight(QFont::Normal);

      QPixmap pixmap(w, captionH);
      pixmap.fill(Qt::black);
      QPainter painter;
      painter.begin(&pixmap);
      painter.setFont(font);

      painter.setPen(Qt::white);
      QRect r(1, 1, w-2, captionH - 2);
      painter.drawText(r, Qt::AlignLeft, caption, -1, &r);
      painter.end();
      QImage fontImage = pixmap.convertToImage();
      QRgb black = QColor(0, 0, 0).rgb();
      for(int srcY = 0; srcY < fontImage.height(); srcY++)
        for(int srcX = 0; srcX < fontImage.width(); srcX++)
        {
          int destX = x1 + left + srcX;
          int destY = y1 + top + h - (captionH + 1) + srcY;

          // adjust the destination coordinates for rotation
          switch(photo->rotation) {
            case 90 : { 
                       destX = left + x1 + (captionH - srcY);
                       destY = top + y1 + srcX;
                       break;
                     }
            case 180 : { 
                       destX = left + x1 + w - srcX;
                       destY = top + y1 + (captionH - srcY);
                       break;
                     }
            case 270 : { 
                       destX = left + x1 + (w - captionH) + srcY;
                       destY = top + y1 + h - srcX;
                       break;
                     }
          }

          if (fontImage.pixel(srcX, srcY) != black)
            p.setPixel(destX, destY, fontImage.pixel(srcX, srcY));
        }
    } // caption

    // iterate to the next position
    layout = layouts.next();
    if (layout == 0)
    {
      current++;
      break;
    }
  }
  // did we print the last photo?
  return (current < photos.count());
}

}  // NameSpace KIPIPrintWizardPlugin


