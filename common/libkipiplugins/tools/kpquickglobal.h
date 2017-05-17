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

#ifndef KPQUICKGLOBAL_H
#define KPQUICKGLOBAL_H

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

namespace KIPIPlugins
{

    // Type of request for special image provider
    enum RequestType {
            RequestThumbnail = 0,
            RequestPreview = 0
    };

    extern QString ThumbnailsImageProvider;
    extern QString PreviewImageProvider;

    // Encodes KIPI-provided url into Base64. To use with image custom image provider
    QString encodeUrl( const QUrl& url);
    QUrl decodeUrl(const QString& encodedUrl);

    QUrl createThumbnailUrl(const QUrl& url);
    QUrl createPreviewUrl(const QUrl& url);
}

#endif
