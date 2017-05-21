/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : image provider for thumbnails/previews obtained via KIPI interface
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

#ifndef KPQUICK_ASYNC_IMAGE_PROVIDER_H
#define KPQUICK_ASYNC_IMAGE_PROVIDER_H

// Qt includes

#include <QQuickAsyncImageProvider>
#include <QUrl>
#include <QObject>

// KIPI includes
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_export.h"
#include "kpquickimagerequestresponse.h"
#include "kpquickglobal.h"

namespace KIPIPlugins
{

/** QuickAsyncImageProvider via KIPI interface
 * */

class KIPIPLUGINS_EXPORT KPQuickAsyncImageProvider : public QQuickAsyncImageProvider
{
public:

    KPQuickAsyncImageProvider( KIPI::Interface* interface,
        RequestType request );
    virtual ~KPQuickAsyncImageProvider() {}

    virtual QQuickImageResponse* requestImageResponse(const QString& id, const QSize& requestedSize);

private:
    KIPI::Interface* m_interface;
    RequestType m_requestType;

};

}

#endif


