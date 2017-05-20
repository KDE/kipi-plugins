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

#include "kpquickasyncimageprovider.h"

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

KPQuickAsyncImageProvider::KPQuickAsyncImageProvider( KIPI::Interface* interface,
	RequestType requestType) :
    QQuickAsyncImageProvider(), m_interface(interface), m_requestType(requestType)
{
	if( interface == 0 ) {
		qCCritical(KIPIPLUGINS_LOG) << "Interface can't be null";
		return;
	}
}

QQuickImageResponse* KPQuickAsyncImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
	QUrl url = decodeUrl(id);

	return new KPQuickImageRequestResponse(m_interface, url,
                requestedSize,
                m_requestType);
}


