/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2002-12-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2002-2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2008 by Angelo Naselli <anaselli at linux dot it>
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

#include "tphoto.h"

// Qt includes

#include <QPainter>
#include <QFileInfo>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

#define IMAGE_FILE_MASK "*"
//"*.jpg;*.jpeg;*.JPG;*.JPEG;*.png;*.PNG"

namespace KIPIPrintImagesPlugin
{

TPhoto::TPhoto(int thumbnailSize)
      : pAddInfo(NULL), pCaptionInfo(NULL)
{
    m_size = 0;
    cropRegion = QRect(-1, -1, -1, -1);
    rotation = 0;
    first = false;

    copies = 1;
    //TODO mPrintPosition;
    filename = "";
    m_exiv2Iface = NULL;

    m_thumbnail = NULL;

    this->m_thumbnailSize = thumbnailSize;
}

//to get old photo info
TPhoto::TPhoto (const TPhoto& photo): pAddInfo(NULL), pCaptionInfo(NULL)
{
  m_thumbnailSize = photo.m_thumbnailSize;
  cropRegion      = photo.cropRegion;
  filename        = photo.filename;
  first           = photo.first;
  copies          = photo.copies;
  rotation        = photo.rotation;
  
  if (photo.pAddInfo)
  {
    pAddInfo = new AdditionalInfo(*photo.pAddInfo);
  }
  if (photo.pCaptionInfo)
  {
    pCaptionInfo = new CaptionInfo(*photo.pCaptionInfo);
  }
  
  m_size       = 0;
  m_exiv2Iface = NULL;
  m_thumbnail  = NULL;
}

TPhoto::~TPhoto()
{
    delete m_thumbnail;
    delete m_size;
    delete m_exiv2Iface;
    delete pAddInfo;
    delete pCaptionInfo;
}

void TPhoto::loadCache()
{
    // load the thumbnail and size only once.
    delete m_thumbnail;

    QImage photo = loadPhoto();
    QImage image = photo.scaled(m_thumbnailSize, m_thumbnailSize, Qt::KeepAspectRatio);

    m_thumbnail = new QPixmap(image.width(), image.height());
    QPainter painter(m_thumbnail);
    painter.drawImage(0, 0, image );
    painter.end();

    if (m_size)
        delete m_size;
    m_size = new QSize(photo.width(), photo.height());
}

QPixmap & TPhoto::thumbnail()
{
    if (!m_thumbnail)
        loadCache();
    return *m_thumbnail;
}

QImage  TPhoto::loadPhoto()
{
    QImage photo;

    // Check if RAW file.
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    QFileInfo fileInfo(filename.path());
    if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper()))
        KDcrawIface::KDcraw::loadDcrawPreview(photo, filename.path());
    else
        photo.load(filename.path()); // PENDING(blackie) handle URL

    return photo;
}

QSize & TPhoto::size()  // private
{
    if (m_size == 0)
        loadCache();
    return *m_size;
}

KExiv2Iface::KExiv2 *TPhoto::exiv2Iface()
{
    if (!m_exiv2Iface && !filename.url().isEmpty())
    {
        m_exiv2Iface = new KExiv2Iface::KExiv2(filename.path());
    }

    return m_exiv2Iface;
}

int TPhoto::width()
{
    return size().width();
}

int TPhoto::height()
{
    return size().height();
}

double TPhoto::scaleWidth(double unitToInches)
{
  Q_ASSERT(pAddInfo != NULL);
  cropRegion = QRect(0, 0,
                     (int)(pAddInfo->mPrintWidth * unitToInches),
                     (int)(pAddInfo->mPrintHeight * unitToInches));
  return pAddInfo->mPrintWidth * unitToInches;
}

double TPhoto::scaleHeight(double unitToInches)
{
  Q_ASSERT(pAddInfo != NULL);
  cropRegion = QRect(0,
                     0,
                     (int)(pAddInfo->mPrintWidth * unitToInches),
                     (int)(pAddInfo->mPrintHeight * unitToInches));
              return pAddInfo->mPrintHeight * unitToInches;
}

}  // NameSpace KIPIPrintImagesPlugin
