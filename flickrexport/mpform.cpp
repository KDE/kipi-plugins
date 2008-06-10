/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
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

// C++ includes.

#include <cstring>
#include <cstdio>

// Qt includes.

#include <qfile.h>
#include <qfileinfo.h>
#include <q3textstream.h>
//Added by qt3to4:
#include <Q3CString>

// KDE includes.

#include <kapplication.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kurl.h>
#include <krandom.h>

// Local includes.

#include "mpform.h"

namespace KIPIFlickrExportPlugin
{

MPForm::MPForm()
{
    m_boundary  = "----------";
    m_boundary += KRandom::randomString(42 + 13).ascii();
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
    Q3CString str;
    str += "--";
    str += m_boundary;
    str += "--";

    Q3TextStream ts(m_buffer, QIODevice::Append|QIODevice::WriteOnly);
    ts.setEncoding(Q3TextStream::UnicodeUTF8);
    ts << str;
}

bool MPForm::addPair(const QString& name, const QString& value)
{
    Q3CString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.ascii();
    str += "\"";
    str += "\r\n\r\n";
    str += value.utf8();
    str += "\r\n";

    //uint oldSize = m_buffer.size();
    //m_buffer.resize(oldSize + str.size());
    //memcpy(m_buffer.data() + oldSize, str.data(), str.size());

    Q3TextStream ts(m_buffer, QIODevice::Append|QIODevice::WriteOnly);
    ts.setEncoding(Q3TextStream::UnicodeUTF8);
    ts << QString::fromUtf8(str);

    return true;
}

bool MPForm::addFile(const QString& name,const QString& path)
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
    imageFile.close();

    Q3CString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.ascii();
    str += "\"; ";
    str += "filename=\"";
    str += QFile::encodeName(KUrl(path).fileName());
    str += "\"";
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.ascii();
    str += "\r\n\r\n";

    Q3TextStream ts(m_buffer, QIODevice::Append|QIODevice::WriteOnly);
    ts.setEncoding(Q3TextStream::UnicodeUTF8);
    ts << str;

    int oldSize = m_buffer.size();
    m_buffer.resize(oldSize + imageData.size() + 2);
    memcpy(m_buffer.data() + oldSize, imageData.data(), imageData.size());
    m_buffer[m_buffer.size()-2] = '\r';
    m_buffer[m_buffer.size()-1] = '\n';

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

} // namespace KIPIFlickrExportPlugin
