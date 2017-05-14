/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
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

#include "kpquickinit.h"

// Qt includes
#include <QQmlEngine>

// Libkipi includes

#include <libkipi_version.h>
#include <KIPI/Interface>
#include <KIPI/ImageInfo>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"
#include "kpquickimageinfo.h"
#include "kpquickimagecollection.h"
#include "kpquickinterface.h"

using namespace KIPI;

namespace KIPIPlugins
{

const char* const kipi_qml_namespace = "com.kde.kipiplugins";
const int kipi_qml_version_major = 0;
const int kipi_qml_version_minor = 1;

void InitKIPIQuick()
{
    qmlRegisterType<KPQuickImageInfo>(kipi_qml_namespace, kipi_qml_version_major,
        kipi_qml_version_minor, "ImageInfo" );
    qmlRegisterUncreatableType<KPQuickImageCollection>(kipi_qml_namespace, kipi_qml_version_major,
        kipi_qml_version_minor, "ImageCollection", QLatin1String("Plugin should never create ImageCollection. Only host can do that") );
    qmlRegisterUncreatableType<KPQuickInterface>(kipi_qml_namespace, kipi_qml_version_major,
        kipi_qml_version_minor, "Interface", QLatin1String("Plugin can't create Interface. It is provided by host") );

}

}
