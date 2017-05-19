/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : part of image provider for thumbnails/previews obtained via KIPI interface
 *
 * Copyright (C) 2017 by Artem Serebriyskiy <v.for.vandal@gmail.com>
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

#ifndef KPQUICK_IMAGE_REQUESTRESPONSE_H
#define KPQUICK_IMAGE_REQUESTRESPONSE_H

// Qt includes

#include <QQuickImageResponse>
#include <QUrl>
#include <QObject>

// KIPI includes
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_export.h"
#include "kpquickglobal.h"

namespace KIPIPlugins
{

/** Request/response class for thumbnails/previes via KIPI interface
 */

class KIPIPLUGINS_EXPORT KPQuickImageRequestResponse : public QQuickImageResponse
{
    Q_OBJECT

public:

    KPQuickImageRequestResponse( KIPI::Interface* interface, const QUrl& url, int size,
		RequestType request = RequestThumbnail );
    virtual ~KPQuickImageRequestResponse() {}

    virtual QQuickTextureFactory* textureFactory() const;

public Q_SLOTS:
    void cancel();

private Q_SLOTS:
    void onGotThumbnail(const QUrl& url, const QPixmap& pixmap);
    void onGotPreview(const QUrl& url, const QImage& image);

private:
    QUrl m_url;
    QImage m_resultImage;

};

}

#endif

