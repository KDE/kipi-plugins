/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local includes

#include "qmlimageviewer.h"
#include "kpaboutdata.h"
#include "kpquickinit.h"

using namespace KIPIPlugins;

/** Test application for qml binding. Shows specified albums
*/
int main(int argc, char* argv[])
{
    KPAboutData* const about = new KPAboutData(ki18n("QmlImageViewer"),
                                               ki18n("A test application to to view images in QML."),
                                               ki18n("(c) 2017-2017, Artem Serebriyskiy\n"));

    about->addAuthor(ki18n("Artem Serebriyskiy").toString(),
                     ki18n("Author").toString(),
                     QLatin1String("v.for.vandal@gmail.com"));

    InitKIPIQuick(); 

    QApplication app(argc, argv);
    KPQmlImageViewer* const viewer = new KPQmlImageViewer(about);
    viewer->show();
    app.exec();
    return 0;
}
