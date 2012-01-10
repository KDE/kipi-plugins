/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Smugmug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mpform.h"

// C++ includes

#include <cstring>
#include <cstdio>

// Qt includes

#include <QFile>

// KDE includes

#include <kapplication.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kurl.h>
#include <krandom.h>

namespace KIPISmugPlugin
{

MPForm::MPForm()
{
    m_boundary  = "----------";
    m_boundary += KRandom::randomString(42 + 13).toAscii();
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
    QByteArray str;
    str += "--";
    str += m_boundary;
    str += "--";
    m_buffer.append(str);
}

bool MPForm::addPair(const QString& name, const QString& value, const QString& contentType)
{
    QByteArray str;
    QString content_length = QString("%1").arg(value.length());

    str += "--";
    str += m_boundary;
    str += "\r\n";

    if (!name.isEmpty()) 
    { 
      	str += "Content-Disposition: form-data; name=\"";
    	str += name.toAscii();
    	str += "\"\r\n";
    }
    if (!contentType.isEmpty()) 
    {
        str += "Content-Type: " + QByteArray(contentType.toAscii());
        str += "\r\n";
    	str += "Mime-version: 1.0 ";
    	str += "\r\n";
    }
    str += "Content-Length: ";
    str += content_length.toAscii();
    str += "\r\n\r\n";
    str += value.toUtf8();

    m_buffer.append(str); 
    m_buffer.append("\r\n");
    return true;
}

bool MPForm::addFile(const QString& name, const QString& path)
{
    KMimeType::Ptr ptr = KMimeType::findByUrl(path);
    QString mime       = ptr->name();
    if (mime.isEmpty())
    {
        // if we ourselves can't determine the mime of the local file,
        // very unlikely the remote site will be able to identify it
        return false;
    }

    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly))
        return false;

    QByteArray imageData = imageFile.readAll();

    QByteArray str;
    QString file_size = QString("%1").arg(imageFile.size());
    imageFile.close();

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.toAscii();
    str += "\"; ";
    str += "filename=\"";
    str += QFile::encodeName(KUrl(path).fileName());
    str += "\"\r\n";
    str += "Content-Length: ";
    str += file_size.toAscii();
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.toAscii();
    str += "\r\n\r\n";

    m_buffer.append(str);
    //int oldSize = m_buffer.size();
    m_buffer.append(imageData);
    m_buffer.append("\r\n");

    return true;
}

QString MPForm::contentType() const
{
    return QString("Content-Type: multipart/form-data; boundary=" + m_boundary);
}

QString MPForm::boundary() const
{
    return m_boundary;
}

QByteArray MPForm::formData() const
{
    return m_buffer;
}

} // namespace KIPISmugPlugin
