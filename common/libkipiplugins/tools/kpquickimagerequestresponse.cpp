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
#include <QTimer>

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
	const QSize& size,
	RequestType requestType ):
    QQuickImageResponse(), m_size(size), m_url(url)
{
    if( interface == 0 ) {
            return;
    }

    if(m_size.isNull()) {
        // No point in calling interface for empty thumbnail
        QTimer::singleShot(0, this, &KPQuickImageRequestResponse::handleNullRequest);
        return;
    }

    int boundSize = 0;

    // if any m_size width or height is 0, then make it equal to other parameters
    if(m_size.width() == 0 ) {
        boundSize = m_size.height();
    } else if(m_size.height() == 0) {
        boundSize = m_size.width();
    } else {
        boundSize = qMax(m_size.width(), m_size.height());
    }

    qCDebug(KIPIPLUGINS_LOG) << "Request for " <<
        (requestType == RequestThumbnail?"thumbnail":"preview")  << url << " size: " << size << " boundSize:" << boundSize;

    switch( requestType ) {
        case RequestPreview:
        case RequestThumbnail:
            connect( interface, &Interface::gotThumbnail, this, &KPQuickImageRequestResponse::onGotThumbnail);
            interface->thumbnail(url, boundSize);
            break;
        //case RequestPreview:
            // TODO: Crushes digikam. Blocking for now
            /*
            connect( interface, &Interface::gotPreview, this, &KPQuickImageRequestResponse::onGotPreview);
            interface->preview(url, boundSize);
            */
            break;
    }
}

void KPQuickImageRequestResponse::onGotThumbnail(const QUrl& url, const QPixmap& pixmap)
{
    /*
    qCDebug(KIPIPLUGINS_LOG) << "Got thumbnail for " <<
        url << " size: " << pixmap.size() << " downscale to: " << m_size;*/ // TODO: REMOVE
    if( url != m_url ) {
            return;
    }

    m_resultImage = scaleImage(pixmap.toImage());
    emit finished();
}

void KPQuickImageRequestResponse::onGotPreview(const QUrl& url, const QImage& image)
{
    qCDebug(KIPIPLUGINS_LOG) << "Got preview for " <<
        url << " size: " << image.size() << " downscale to: " << m_size;
    if( url != m_url ) {
            return;
    }

    m_resultImage = scaleImage(image);
    emit finished();
}

QImage KPQuickImageRequestResponse::scaleImage(const QImage& image)
{
    if(m_size.width() == 0 ) {
        return image.scaledToHeight(m_size.height());
    } else if(m_size.height() == 0) {
        return image.scaledToWidth(m_size.width());
    } else {
        return image.scaled(m_size);
    }
}

void KPQuickImageRequestResponse::cancel()
{
    m_url = QUrl();
    emit finished();
}

// When no call to interface was made, but we need to emit 'finished' signal
void KPQuickImageRequestResponse::handleNullRequest()
{
    emit finished();
}

QQuickTextureFactory* KPQuickImageRequestResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_resultImage);
}
