/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : Global definitions for KIPI plugins QML wrappers
 *
 * Copyright (C) 2012-2017 by Artem Serebriyskiy <v.for.vandal@gmail.com>
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

#include "kpquickglobal.h"

// Qt includes

#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <QObject>

// Local includes

#include "kipiplugins_export.h"
#include "kpimageinfo.h"

using namespace KIPI;

namespace KIPIPlugins {

QString ThumbnailsImageProvider( QStringLiteral("KIPIThumbnail"));
QString PreviewImageProvider( QStringLiteral("KIPIPreview"));

// Encodes KIPI-provided url into Base64. To use with image custom image provider
QString encodeUrl( const QUrl& url)
{
    QByteArray plainUrl = url.toEncoded();
    QByteArray encodedUrl = plainUrl.toBase64();
    return QString(encodedUrl);
}

QUrl decodeUrl(const QString& encodedUrl)
{
    // id is base64-encoded KIPI-provided url
    QByteArray urlEncodedArray = encodedUrl.toUtf8();
    QByteArray urlDecoded = QByteArray::fromBase64(urlEncodedArray);
    QString urlString = QString(urlDecoded);
    QUrl url = QUrl(urlString);
    return url;
}

static QUrl createProviderUrl( const QUrl& url, const QString& provider )
{
    QString data = encodeUrl(url);
    QUrl result;
    result.setScheme("image");

    QString path = provider + '/' + data;
    result.setPath(path);

    return result;
}

QUrl createThumbnailUrl(const QUrl& url)
{
    return createProviderUrl(url, ThumbnailsImageProvider);
}

QUrl createPreviewUrl(const QUrl& url)
{
    return createProviderUrl(url, PreviewImageProvider);
}

}
