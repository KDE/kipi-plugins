/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2015-09-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#include "kpmultipart.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIPlugins
{

KPMultiPart::KPMultiPart()
    : m_multiPart(new QHttpMultiPart(QHttpMultiPart::FormDataType))
{
}

KPMultiPart::~KPMultiPart()
{
    delete m_multiPart;
}

bool KPMultiPart::appendPair(const QString& name, const QString& value, const QString& contentType)
{
    QHttpPart metaPart;
    metaPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"%1\"").arg(name)));
    metaPart.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(QString("%1").arg(value.length())));
    metaPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(contentType));

    metaPart.setBody(QByteArray(value.toUtf8()));
    m_multiPart->append(metaPart);
    return true;
}

bool KPMultiPart::appendFile(const QString& header, const QString& path)
{
    QString mime = QMimeDatabase().mimeTypeForUrl(QUrl(path)).name();

    if (mime.isEmpty())
        return false;

    QFileInfo fileInfo(path);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString("form-data; name=\"%1\"; filename=\"%2\"").arg(header).arg(fileInfo.fileName())));
    imagePart.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(fileInfo.size()));
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mime));
    QFile* const file = new QFile(path);

    if (!file->open(QIODevice::ReadOnly))
    {
        delete file;
        return false;
    }

    imagePart.setBodyDevice(file);
    file->setParent(m_multiPart); // we cannot delete the file now, so delete it with the multiPart

    m_multiPart->append(imagePart);
    return true;
}

QHttpMultiPart* KPMultiPart::multiPart() const
{
    return m_multiPart;
}

}   // namespace KIPIPlugins
