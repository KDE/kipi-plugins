/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2006-2010 Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2010-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpaboutdata.h"

// KDE includes

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kdebug.h>

// Local includes

#include "pluginsversion.h"

namespace KIPIPlugins
{

KPAboutData::KPAboutData(const KLocalizedString& pluginName,
                         const QByteArray& /*pluginVersion*/,
                         enum LicenseKey licenseType,
                         const KLocalizedString& pluginDescription,
                         const KLocalizedString& copyrightStatement)
           : KAboutData (QByteArray("kipiplugins"),  // Name without minus separator for KDE bug report.
                         QByteArray(),
                         pluginName,
                         kipipluginsVersion().toAscii(),
                         KLocalizedString(),
                         licenseType,
                         copyrightStatement,
                         pluginDescription,
                         QByteArray("http://www.kipi-plugins.org"))
{
    if (KGlobal::hasMainComponent())
    {
        // setProgramLogo is defined from kde 3.4.0 on
        QString directory = KStandardDirs::locate("data", "kipi/data/kipi-plugins_logo.png");

        // set the kipiplugins logo inside the about dialog
        setProgramLogo(QImage(directory));
    }

    // set the plugin description into long text description
    setOtherText(pluginDescription);

    // put the plugin name and version with kipi-plugins and kipi-plugins version
    KLocalizedString shortDesc = additionalInformation();

    if (KGlobal::hasMainComponent())
    {
        kDebug(AREA_CODE_LOADING) << shortDesc.toString().constData() ;
    }

    // and set the string into the short description
    setShortDescription(shortDesc);
}

}   // namespace KIPIPlugins
