/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
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

// C++ includes.

#include <cstring>
#include <cstdio>

// Qt includes.

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

// KDE includes.

#include <kapplication.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kurl.h>

// Local includes.

#include "mpform.h"

namespace KIPIPicasawebExportPlugin
{

MPForm::MPForm()
{
    m_boundary  = "----------";
    m_boundary += KApplication::randomString(42 + 13).ascii();
}

MPForm::~MPForm()
{
}

void MPForm::reset()
{
    m_buffer.resize(0);
}

void MPForm::finish()
{
    QCString str;
    str += "--";
    str += m_boundary;
    str += "--";

    QTextStream ts(m_buffer, IO_Append|IO_WriteOnly);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    ts << str;
}

bool MPForm::addPair(const QString& name, const QString& value, const QString& contentType)
{
    QCString str;
    QString  content_length = QString("%1").arg(value.length());
    str += "--";
    str += m_boundary;
    str += "\r\n";

    if (!name.isEmpty())
    {
        str += "Content-Disposition: form-data; name=\"";
        str += name.ascii();
        str += "\"\r\n";
    }

    if (!contentType.isEmpty())
    {
        str += "Content-Type: "+ QCString(contentType.ascii());
        str += "\r\n";
        str += "Mime-version: 1.0 ";
        str += "\r\n";
    }

    str += "Content-Length: " ;
    str += content_length.ascii();
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

bool MPForm::addFile(const QString& name,const QString& path)
{
    KMimeType::Ptr ptr = KMimeType::findByURL(path);
    QString mime       = ptr->name();

    if (mime.isEmpty())
    {
        // if we ourselves can't determine the mime of the local file,
        // very unlikely the remote site will be able to identify it
        return false;
    }

    QFile imageFile(path);
    if ( !imageFile.open( IO_ReadOnly ) )
        return false;

    QByteArray imageData = imageFile.readAll();

    QCString str;
    QString file_size = QString("%1").arg(imageFile.size());

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.ascii();
    str += "\"; filename=\"";
    str += QFile::encodeName(KURL(path).filename());
    str += "\"\r\n"; 
    str += "Content-Length: " ;
    str +=  file_size.ascii();
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.ascii();
    str += "\r\n\r\n";

    imageFile.close();
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

QString MPForm::contentType() const
{
    return QString("Content-Type: multipart/related; boundary=" + m_boundary);
}

QString MPForm::boundary() const
{
    return m_boundary;
}

QByteArray MPForm::formData() const
{
    return m_buffer;
}

} // namespace KIPIPicasawebExportPlugin
