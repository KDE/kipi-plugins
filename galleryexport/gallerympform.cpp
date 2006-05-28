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
 * either version 2, or (at your option)
 * any later version.
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

#include <cstring>
#include <cstdio>

#include "gallerympform.h"

namespace KIPIGalleryExportPlugin
{

GalleryMPForm::GalleryMPForm()
{
    m_boundary  = "----------";
    m_boundary += KApplication::randomString( 42 + 13 ).ascii();
    m_using_gallery2 = false;
}

GalleryMPForm::~GalleryMPForm()
{
}

void GalleryMPForm::reset()
{
    m_buffer.resize(0);
}

void GalleryMPForm::finish()
{
    QCString str;
    str += "--";
    str += m_boundary;
    str += "--";
    str += "\r\n";

    QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    ts << str;
}

bool GalleryMPForm::addPair(const QString& name, const QString& value)
{
    QCString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.ascii();
    str += "\"";
    str += "\r\n\r\n";
    str += value.ascii();
    str += "\r\n";

    //uint oldSize = m_buffer.size();
    //m_buffer.resize(oldSize + str.size());
    //memcpy(m_buffer.data() + oldSize, str.data(), str.size());

    QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    ts << str;

    return true;
}

bool GalleryMPForm::addFile(const QString& path)
{
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

    QCString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    if (m_using_gallery2)
      str += "g2_userfile";
    else
      str += "userfile";
    str += "\"; ";
    str += "filename=\"";
    str += QFile::encodeName(KURL(path).filename());
    str += "\"";
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.ascii();
    str += "\r\n\r\n";

    QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    ts << str;

    int oldSize = m_buffer.size();
    m_buffer.resize(oldSize + imageData.size() + 2);
    memcpy(m_buffer.data()+oldSize, imageData.data(), imageData.size());
    m_buffer[m_buffer.size()-2] = '\r';
    m_buffer[m_buffer.size()-1] = '\n';

    return true;
}

QString GalleryMPForm::contentType() const
{
    return QString("Content-Type: multipart/form-data; boundary=" + m_boundary);
}

QString GalleryMPForm::boundary() const
{
    return m_boundary;
}

QByteArray GalleryMPForm::formData() const
{
    return m_buffer;
}

}
