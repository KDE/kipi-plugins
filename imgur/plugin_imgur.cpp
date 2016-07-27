/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#include "plugin_imgur.h"

// Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPIImgurPlugin
{

K_PLUGIN_FACTORY( ImgurFactory, registerPlugin<Plugin_Imgur>(); )

class Plugin_Imgur::Private
{
public:

    Private()
    {
        actionExport = 0;
        winExport    = 0;
    }

    QAction*     actionExport;
    ImgurWindow* winExport;
};

Plugin_Imgur::Plugin_Imgur(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Imgur"),
      d(new Private)
{
    qCDebug(KIPIPLUGINS_LOG) << "Imgur plugin loaded";

    setUiBaseName("kipiplugin_imgurui.rc");
    setupXML();
}

Plugin_Imgur::~Plugin_Imgur()
{
    delete d->winExport;
    delete d;
}

void Plugin_Imgur::setup(QWidget* const widget)
{
    d->winExport = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_Imgur::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new QAction(this);
    d->actionExport->setText(i18n("Export to &Imgur..."));
    d->actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-imgur")));

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(QString::fromLatin1("imgurexport"), d->actionExport);
}

void Plugin_Imgur::slotActivate()
{
    if (!d->winExport)
    {
        // We clean it up in the close button
        d->winExport = new ImgurWindow(QApplication::activeWindow());
    }
    else
    {
        if (d->winExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(d->winExport->winId());
        }

        KWindowSystem::activateWindow(d->winExport->winId());
    }

    d->winExport->reactivate();

    qCDebug(KIPIPLUGINS_LOG) << "We have activated the imgur exporter!";
}

} // namespace KIPIImgurPlugin

#include "plugin_imgur.moc"
