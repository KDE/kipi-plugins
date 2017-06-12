/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "photolayoutswindow.h"
#include "PLEAboutData.h"

#include <QDesktopWidget>
#include <QResource>
#include <QString>

#include <klocalizedstring.h>
#include <QApplication>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <QIcon>

using namespace PhotoLayoutsEditor;

int main(int argc, char* argv[])
{
    PLEAboutData aboutData;
    aboutData.setAppName("photolayoutseditor");
    aboutData.setCatalogName("kipiplugin_photolayouteditor");

    KCmdLineArgs::init(argc,argv,&aboutData);
    KCmdLineOptions options;
    options.add("+file", ki18n("Input file"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;
    aboutData.setProgramLogo(QIcon::fromTheme("photolayoutseditor"));

    PhotoLayoutsWindow* w = PhotoLayoutsEditor::instance(0);
    w->setAttribute(Qt::WA_DeleteOnClose, true);

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if (args->count())
    {
        QUrl url = args->url(0);
        if (url.isValid())
            w->open(url);
    }

    w->show();

    int result = app.exec();

    return result;
}
