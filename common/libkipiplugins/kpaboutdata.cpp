/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 * 
 * Copyright (C) 2006 Angelo Naselli <anaselli at linux dot it>
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

// Qt includes.

#include <QGlobal>

// KDE includes.

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kdeversion.h>

// Local includes.

#include "kdebug.h"
#include "pluginsversion.h"
#include "kpaboutdata.h"

namespace KIPIPlugins
{

KPAboutData::KPAboutData(const char *pluginName,
                         const char *pluginVersion,
                         int licenseType,
                         const char *pluginDescription,
                         const char *copyrightStatement) 
           : KAboutData ("kipi-plugins", 
                         pluginName,
                         pluginVersion,
                         0, licenseType,
                         copyrightStatement,
                         pluginDescription,
                         "http://www.kipi-plugins.org")
{
    // setProgramLogo is defined from kde 3.4.0 on
    QString directory;
    KGlobal::dirs()->addResourceType("kipi_plugins_logo", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_plugins_logo", "kipi-plugins_logo.png");

    // set the kipiplugins logo inside the about dialog
    setProgramLogo(QImage(directory + "kipi-plugins_logo.png"));
#endif
    // set the plugin description into long text description
    setOtherText(pluginDescription);

    // put the plugin name and version with kipi-plugins and kipi-plugins version 
    KipiPluginsVersionString = QString(pluginName) + " " + QString(pluginVersion) + 
                              " - Kipi-plugins " + QString(kipiplugins_version) ;

    kdDebug( 51001 ) << "Kipi-plugins - " << KipiPluginsVersionString.ascii() << endl;

    // and set the string into the short description
    setShortDescription(KipiPluginsVersionString.ascii());
}

}   // namespace KIPIPlugins
