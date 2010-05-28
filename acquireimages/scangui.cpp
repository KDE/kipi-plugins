/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-30
 * Description : stand alone Scanner interface.
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <kicon.h>
#include <kconfig.h>
#include <kglobal.h>

// Libkexiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// LibKSane includes

#include <libksane/ksane.h>

// Local includes

#include "aboutdata.h"
#include "pluginsversion.h"
#include "scandialog.h"

using namespace KIPIAcquireImagesPlugin;
using namespace KExiv2Iface;
using namespace KSaneIface;

int main(int argc, char* argv[])
{
    ScanDialogAboutData* aboutData = new ScanDialogAboutData;
    aboutData->setAppName("scangui");
    aboutData->setCatalogName("kipiplugin_acquireimages");
    KCmdLineArgs::init(argc, argv, aboutData);

    KExiv2::initializeExiv2();

    KApplication app;
    aboutData->setProgramLogo(KIcon("scanner"));

    KSaneWidget* saneWidget = new KSaneWidget(0);
    if (!saneWidget)
      return -1;

    QString dev = saneWidget->selectDevice(0);
    if (dev.isEmpty())
        return -1;

    if (!saneWidget->openDevice(dev))
    {
        // could not open a scanner
        KMessageBox::sorry(0, i18n("Cannot open scanner device."));
        return -1;
    }

    ScanDialog* dlg = new ScanDialog(0, saneWidget, app.activeWindow(), aboutData);
    app.setTopWidget(dlg);
    dlg->show();


    int ret = app.exec();

    KExiv2::cleanupExiv2();

    return ret;
}
