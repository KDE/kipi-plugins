/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
 *
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

#include "gallerympform.h"

// C++ includes

#include <cstring>
#include <cstdio>

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>
#include <kmimetype.h>
#include <krandom.h>
#include <kurl.h>

// Local includes

#include "gallerytalker.h"

namespace KIPIGalleryExportPlugin
{

GalleryMPForm::GalleryMPForm()
{
    m_boundary  = "----------";
    m_boundary += KRandom::randomString( 42 + 13 ).toAscii();

    if (GalleryTalker::isGallery2())
    {
        addPairRaw("g2_controller", "remote:GalleryRemote");
        QString auth_token = GalleryTalker::getAuthToken();
        if (!auth_token.isEmpty())
            addPairRaw("g2_authToken", auth_token);
    }
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
    QString str;
    str += "--";
    str += m_boundary;
    str += "--";
    str += "\r\n";

    m_buffer.append(str.toUtf8());
}

bool GalleryMPForm::addPair(const QString& name, const QString& value)
{
    if (GalleryTalker::isGallery2())
        return addPairRaw(QString("g2_form[%1]").arg(name), value);

    return addPairRaw(name, value);
}

bool GalleryMPForm::addPairRaw(const QString& name, const QString& value)
{
    QString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";
    str += name.toAscii();
    str += "\"";
    str += "\r\n\r\n";
    str += value.toAscii();
    str += "\r\n";

    m_buffer.append(str.toUtf8());
    return true;
}

bool GalleryMPForm::addFile(const QString& path, const QString& displayFilename)
{

    QString filename = "userfile_name";
    if (GalleryTalker::isGallery2())
        filename = "g2_userfile_name";

    if (!addPairRaw(filename, displayFilename)) 
    {
        return false;
    }

    KMimeType::Ptr ptr = KMimeType::findByUrl(path);
    QString mime = ptr->name();
    if (mime.isEmpty())
    {
        // if we ourselves can't determine the mime of the local file,
        // very unlikely the remote gallery will be able to identify it
        return false;
    }

    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly))
        return false;
    QByteArray imageData = imageFile.readAll();
    imageFile.close();

    QString str;

    str += "--";
    str += m_boundary;
    str += "\r\n";
    str += "Content-Disposition: form-data; name=\"";

    if (GalleryTalker::isGallery2())
        str += "g2_userfile";
    else
        str += "userfile";

    str += "\"; ";
    str += "filename=\"";
    str += QFile::encodeName(KUrl(path).fileName());
    str += "\"";
    str += "\r\n";
    str += "Content-Type: ";
    str +=  mime.toAscii();
    str += "\r\n\r\n";

    m_buffer.append(str.toUtf8());

    int oldSize = m_buffer.size();
    m_buffer.resize(oldSize + imageData.size() + 2);
    memcpy(m_buffer.data() + oldSize, imageData.data(), imageData.size());
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

} // namespace KIPIGalleryExportPlugin
