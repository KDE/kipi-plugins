/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : 
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

#include "kpquickimagerequestresponse.h"

// Qt includes

#include <QMap>
#include <QVariant>

// Libkipi includes

#include <libkipi_version.h>
#include <KIPI/Interface>
#include <KIPI/ImageInfo>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

using namespace KIPI;
using namespace KIPIPlugins;

KPQuickImageRequestResponse::KPQuickImageRequestResponse( KIPI::Interface* interface,
	const QUrl& url,
	int size,
	RequestType requestType ):
    QQuickImageResponse(), m_url(url)
{
    if( interface == 0 ) {
            return;
    }
    
    qCDebug(KIPIPLUGINS_LOG) << "Request for image: " << url << " size: " << size;
    switch( requestType ) {
        case RequestThumbnail:
            connect( interface, &Interface::gotThumbnail, this, &KPQuickImageRequestResponse::onGotThumbnail);
            interface->thumbnail(url, size);
            break;
        case RequestPreview:
            connect( interface, &Interface::gotPreview, this, &KPQuickImageRequestResponse::onGotPreview);
            interface->preview(url, size);
            break;
    }
}

void KPQuickImageRequestResponse::onGotThumbnail(const QUrl& url, const QPixmap& pixmap)
{
    qCDebug(KIPIPLUGINS_LOG) << "Got thumbnail for " << url << " size: " << pixmap.size();
    if( url != m_url ) {
            return;
    }

    m_resultImage = pixmap.toImage();
    emit finished();
}

void KPQuickImageRequestResponse::onGotPreview(const QUrl& url, const QImage& image)
{
    qCDebug(KIPIPLUGINS_LOG) << "Got preview for " << url << " size: " << image.size();
    if( url != m_url ) {
            return;
    }

    m_resultImage = image;
    emit finished();
}

void KPQuickImageRequestResponse::cancel()
{
    m_url = QUrl();
    emit finished();
}

QQuickTextureFactory* KPQuickImageRequestResponse::textureFactory() const
{
	return QQuickTextureFactory::textureFactoryForImage(m_resultImage);
}
