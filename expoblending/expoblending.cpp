/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : stand alone Expo Blending.
 *
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

#include "aboutdata.h"
#include "pluginsversion.h"
#include "manager.h"

using namespace KIPIExpoBlendingPlugin;
using namespace KExiv2Iface;

int main(int argc, char* argv[])
{
    ExpoBlendingAboutData* aboutData = new ExpoBlendingAboutData;
    aboutData->setAppName("expoblending");
    aboutData->setCatalogName("kipiplugin_expoblending");

    KCmdLineArgs::init(argc, argv, aboutData);

    KCmdLineOptions options;
    options.add("+[file(s)]", ki18n("File(s) to open"));
    KCmdLineArgs::addCmdLineOptions( options );

    KExiv2::initializeExiv2();

    KApplication app;
    aboutData->setProgramLogo(KIcon("expoblending"));

    KUrl::List urlList;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    for(int i = 0; i < args->count(); ++i)
    {
        urlList.append(args->url(i));
    }
    args->clear();

    Manager mngr;
    if (!mngr.checkBinaries())
        return -1;
    mngr.setItemsList(urlList);
    mngr.setAbout(aboutData);
    mngr.setIface(0);
    mngr.run();

    app.exec();
    KExiv2::cleanupExiv2();

    return 0;
}
