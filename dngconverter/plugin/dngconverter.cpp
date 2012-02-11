/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-30
 * Description : stand alone DNG converter.
 *
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kicon.h>
#include <kconfig.h>
#include <kglobal.h>

// Libkexiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "aboutdata.h"
#include "kpversion.h"
#include "batchdialog.h"

using namespace KIPIDNGConverterPlugin;
using namespace KExiv2Iface;

int main(int argc, char* argv[])
{
    DNGConverterAboutData* aboutData = new DNGConverterAboutData;
    aboutData->setAppName("dngconverter");
    aboutData->setCatalogName("kipiplugin_dngconverter");

    KCmdLineArgs::init(argc, argv, aboutData);

    KCmdLineOptions options;
    options.add("+[file(s)]", ki18n("File(s) to convert"));
    KCmdLineArgs::addCmdLineOptions(options);

    KExiv2::initializeExiv2();

    KApplication app;

    BatchDialog* converter = new BatchDialog(0, aboutData);
    app.setTopWidget(converter);

    KUrl::List urls;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    for(int i = 0; i < args->count(); ++i)
    {
        KUrl url = args->url(i);
        if (DNGConverterAboutData::isRAWFile(url.path()))
            urls.append(url);
    }
    args->clear();

    converter->addItems(urls);
    converter->show();

    int ret = app.exec();

    KExiv2::cleanupExiv2();

    return ret;
}
