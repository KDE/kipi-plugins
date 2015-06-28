/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#include "mpform_picasa.h"

// C++ includes

#include <cstring>
#include <cstdio>

// Qt includes

#include <QByteArray>
#include <QFile>

// KDE includes

#include <kapplication.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <krandom.h>
#include <kurl.h>

namespace KIPIGoogleServicesPlugin
{

MPForm_Picasa::MPForm_Picasa()
{
    m_boundary  = "----------";
    m_boundary += KRandom::randomString(42 + 13).toAscii();
}

MPForm_Picasa::~MPForm_Picasa()
{
}

void MPForm_Picasa::reset()
{
    m_buffer.resize(0);
}

void MPForm_Picasa::finish()
{
    QString str;
    str += "--";
    str += m_boundary;
    str += "--";

    m_buffer.append(str.toUtf8());
}

bool MPForm_Picasa::addPair(const QString& name, const QString& value, const QString& contentType)
{
    QByteArray str;
    QString  content_length = QString("%1").arg(value.length());
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
        str += "Content-Type: "+ QByteArray(contentType.toAscii());
        str += "\r\n";
        str += "Mime-version: 1.0 ";
        str += "\r\n";
    }

    str += "Content-Length: " ;
    str += content_length.toAscii();
    str += "\r\n\r\n";
    str += value.toUtf8();
    str += "\r\n";

    m_buffer.append(str);
    return true;
}

bool MPForm_Picasa::addFile(const QString& name,const QString& path)
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

    QString str;
    QString file_size = QString("%1").arg(imageFile.size());

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.toAscii();
    str += "\"; ";
    str += "filename=\"";
    str += QFile::encodeName(KUrl(path).fileName());
    str += "\"\r\n";
    str += "Content-Length: " ;
    str +=  file_size.toAscii();
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.toAscii();
    str += "\r\n\r\n";

    imageFile.close();
    m_buffer.append(str.toUtf8());

    int oldSize = m_buffer.size();
    m_buffer.resize(oldSize + imageData.size() + 2);
    memcpy(m_buffer.data() + oldSize, imageData.data(), imageData.size());
    m_buffer[m_buffer.size()-2] = '\r';
    m_buffer[m_buffer.size()-1] = '\n';

    return true;
}

QString MPForm_Picasa::contentType() const
{
    return QString("Content-Type: multipart/related; boundary=" + m_boundary);
}

QString MPForm_Picasa::boundary() const
{
    return m_boundary;
}

QByteArray MPForm_Picasa::formData() const
{
    return m_buffer;
}

} // namespace KIPIGoogleServicesPlugin
