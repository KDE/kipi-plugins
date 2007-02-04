/* ============================================================
 * File  : gallerympform.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-02
 * Description :
 *
 * Copyright 2004 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <kapplication.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kurl.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

//#include <cstring>
//#include <cstdio>

#include "galleryform.h"

namespace KIPISyncPlugin
{

GalleryForm::GalleryForm(GalleryVersion version, QString authToken)
 : mVersion(version)
{
  mBoundary  = "----------";
  mBoundary += KApplication::randomString( 42 + 13 ).ascii();

  if (Gallery2 == mVersion)
  {
    addPairRaw("g2_controller", "remote:GalleryRemote");
    if (!authToken.isEmpty())
      addPairRaw("g2_authToken", authToken);
  }
}


GalleryForm::~GalleryForm()
{
}


void GalleryForm::addPair(const QString& name, const QString& value)
{
  if (Gallery2 == mVersion)
    return addPairRaw(QString("g2_form[%1]").arg(name), value);

  return addPairRaw(name, value);
}


void GalleryForm::addPairRaw(const QString& name, const QString& value)
{
  QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
  ts.setEncoding(QTextStream::UnicodeUTF8);
  ts << "--" << mBoundary << "\r\n";
  ts << "Content-Disposition: form-data; name=\"" << name.ascii() << "\"\r\n\r\n";
  ts << value.ascii() << "\r\n";
}

bool GalleryForm::addFile(const QString& path, const QString& displayFilename)
{
  QString filename = "userfile_name";
  if (Gallery2 == mVersion)
    filename = "g2_userfile_name";

  addPairRaw(filename, displayFilename);
  KMimeType::Ptr ptr = KMimeType::findByURL(path);
  QString mime = ptr->name();
  if (mime.isEmpty())
  {
    // if we ourselves can't determine the mime of the local file,
    // very unlikely the remote gallery will be able to identify it
    return false;
  }

  QFile imageFile(path);
  if ( !imageFile.open( IO_ReadOnly ) )
    return false;
  QByteArray imageData = imageFile.readAll();
  imageFile.close();

  QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
  ts.setEncoding(QTextStream::UnicodeUTF8);
  ts << "--" << mBoundary << "\r\n";
  ts << "Content-Disposition: form-data; name=\"";
  if (Gallery2 == mVersion)
    ts << "g2_userfile";
  else
    ts << "userfile";
  ts << "\"; filename=\"" << QFile::encodeName(KURL(path).filename()) << "\"\r\n";
  ts << "Content-Type: " << mime.ascii() << "\r\n\r\n";

  int oldSize = m_buffer.size();
  m_buffer.resize(oldSize + imageData.size() + 3);
  memcpy(m_buffer.data()+oldSize, imageData.data(), imageData.size());
  m_buffer[m_buffer.size()-3] = '\r';
  m_buffer[m_buffer.size()-2] = '\n';
  m_buffer[m_buffer.size()-1] = '\0';
  
  return true;
}


QString GalleryForm::contentType() const
{
  return QString("Content-Type: multipart/form-data; boundary=" + mBoundary);
}


QByteArray GalleryForm::formData()
{
  QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
  ts.setEncoding(QTextStream::UnicodeUTF8);
  ts << "--" << mBoundary << "--" << "\r\n";

  return m_buffer;
}

}
