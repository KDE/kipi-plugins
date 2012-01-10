/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// KDE includes

#include <kmessagebox.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kurl.h>
#include <kicon.h>
#include <kconfig.h>
#include <kglobal.h>

// Libkexiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "pluginsversion.h"
#include "aboutdata.h"
#include "manager/manager.h"

using namespace KIPIPanoramaPlugin;
using namespace KExiv2Iface;

int main(int argc, char* argv[])
{
    PanoramaAboutData* aboutData = new PanoramaAboutData;
    aboutData->setAppName("panorama");
    aboutData->setCatalogName("kipiplugin_panorama");

    KCmdLineArgs::init(argc, argv, aboutData);

    KCmdLineOptions options;
    options.add("+[file(s)]", ki18n("File(s) to open"));
    KCmdLineArgs::addCmdLineOptions( options );

    KExiv2::initializeExiv2();

    KApplication app;
    aboutData->setProgramLogo(KIcon("layer-visible-on"));

    KUrl::List urlList;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    for(int i = 0; i < args->count(); ++i)
    {
        urlList.append(args->url(i));
    }
    args->clear();

    Manager mngr;
    //if (!mngr.checkBinaries())
    //    return -1;

    mngr.setItemsList(urlList);
    mngr.setAbout(aboutData);
    mngr.setIface(0);
    mngr.run();

    app.exec();
    KExiv2::cleanupExiv2();

    return 0;
}
