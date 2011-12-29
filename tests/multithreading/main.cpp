/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy@gmail.com>
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

// KDE includes

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

// Local includes

#include "imageselector.h"

/** Implements rotation of images using threadWeaver api
    rotates the selected images by 180.
*/
int main(int argc, char* argv[])
{
    const KAboutData aboutData("multiThreadTest", "multiThreadTest", ki18n("multiThreadTest"), "1.0");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication app;
    //A simple gui to select images
    ImageSelector* selector = new ImageSelector();
    selector->show();
    app.exec();
    return 0;
}
