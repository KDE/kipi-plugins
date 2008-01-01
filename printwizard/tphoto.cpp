/***************************************************************************
                          tphoto.cpp  -  description
                             -------------------
    begin                : Thu Sep 12 2002
    copyright            : (C) 2002 by Todd Shoemaker
                         : (C) 2007 Angelo Naselli
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
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

// Local includes.

#include "tphoto.h"
#include "utils.h"

// LibKDcraw includes.

#include <libkdcraw/dcrawbinary.h>
#include <libkdcraw/kdcraw.h>


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
  m_exiv2Iface = NULL;

  m_thumbnail = NULL;

  this->m_thumbnailSize = thumbnailSize;
}

TPhoto::~TPhoto()
{
  if (m_thumbnail)
    delete m_thumbnail;
  if (m_size)
    delete m_size;
  if (m_exiv2Iface)
    delete m_exiv2Iface;
}

void TPhoto::loadCache()
{
  // load the thumbnail and size only once.
  if (m_thumbnail)
    delete m_thumbnail;


  QImage photo = loadPhoto();

  m_thumbnail = new QPixmap(QImage( photo.scale(m_thumbnailSize, m_thumbnailSize, QImage::ScaleMin) ));

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
  QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
  QFileInfo fileInfo(filename.path());
  if (rawFilesExt.upper().contains( fileInfo.extension(false).upper() ))
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

}  // NameSpace KIPIPrintWizardPlugin


