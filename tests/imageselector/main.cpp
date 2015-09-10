/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QApplication>
#include <QCommandLineParser>

// Local includes

#include "imageselector.h"
#include "kpaboutdata.h"

using namespace KIPIPlugins;

/** Implements rotation of images using threadWeaver api
    rotates the selected images by 180.
*/
int main(int argc, char* argv[])
{
    KPAboutData* const about = new KPAboutData(ki18n("ImageSelector"), 0,
                                               KAboutLicense::GPL,
                                               ki18n("A test application to rotate list of images."),
                                               ki18n("(c) 2011-2014, Gilles Caulier\n"
                                                     "(c) 2011-2012, A Janardhan Reddy"));

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Author").toString(),
                     QLatin1String("caulier dot gilles at gmail dot com"));

    about->addAuthor(ki18n("A Janardhan Reddy").toString(),
                     ki18n("Author").toString(),
                     QLatin1String("annapareddyjanardhanreddy at gmail dot com"));

    QCommandLineParser parser;
    about->setupCommandLine(&parser);

    QApplication app(argc, argv);
    ImageSelector* const selector = new ImageSelector(about);
    selector->show();
    app.exec();
    return 0;
}
