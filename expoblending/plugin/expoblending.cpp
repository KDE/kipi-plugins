/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : stand alone Expo Blending.
 *
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

// Qt includes

#include <QtWidgets/QApplication>
#include <QCommandLineParser>
#include <QUrl>
#include <QIcon>

// KDE includes

#include <KLocalizedString>

// Local includes

#include "aboutdata.h"
#include "kpmetadata.h"
#include "kpversion.h"
#include "manager.h"

using namespace KIPIExpoBlendingPlugin;
using namespace KIPIPlugins;

int main(int argc, char* argv[])
{
    KLocalizedString::setApplicationDomain("kipiplugin_expoblending");
    QApplication app(argc, argv);

    ExpoBlendingAboutData aboutData;

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("files"), i18n("File(s) to open"), QStringLiteral("+[file(s)]"));
    aboutData.setupCommandLine(&parser);
    aboutData.setProgramLogo(QIcon::fromTheme(QStringLiteral("kipi-expoblending")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    KPMetadata::initializeExiv2();

    QList<QUrl> urlList;
    const QStringList args = parser.positionalArguments();

    for (auto& arg: args)
    {
        urlList.append(QUrl(arg));
    }

    Manager mngr;

    mngr.setItemsList(urlList);
    mngr.setIface(0);
    mngr.run();

    app.exec();
    KPMetadata::cleanupExiv2();

    return 0;
}
