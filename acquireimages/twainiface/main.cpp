/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes.

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

// Libkexiv2 includes.

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes.

#include "twaincontroller.h"
#include "pluginsversion.h"

using namespace KIPIAcquireImagesPlugin;
using namespace KExiv2Iface;

int main(int argc, char *argv[])
{
    KAboutData aboutData("acquireimages", 
                         "kipiplugin_acquireimages",
                         ki18n("Acquire images"),
                         QByteArray(kipiplugins_version),
                         ki18n("A Kipi plugin to acquire images using a flat scanner"),
                         KAboutData::License_GPL,
                         ki18n("(c) 2008, Gilles Caulier"),
                         KLocalizedString(),
                         "http://www.kipi-plugins.org");

    aboutData.addAuthor(ki18n("Gilles Caulier"),
                         ki18n("Author"),
                               "caulier dot gilles at gmail dot com");

    KCmdLineArgs::init(argc, argv, &aboutData);

#if KEXIV2_VERSION >= 0x000300
    KExiv2::initializeExiv2();
#endif

    KApplication app;

    TwainController *twainCtrl = new TwainController;
    app.setTopWidget(twainCtrl);

    int ret = app.exec();

#if KEXIV2_VERSION >= 0x000300
    KExiv2::cleanupExiv2();
#endif

    return ret;
}
