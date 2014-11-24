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

#include "plugin_imgurexport.moc"

// C++ includes

#include <unistd.h>

// KDE includes

#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

using namespace KIPIPlugins;

namespace KIPIImgurExportPlugin
{

K_PLUGIN_FACTORY( ImgurExportFactory, registerPlugin<Plugin_ImgurExport>(); )
K_EXPORT_PLUGIN ( ImgurExportFactory("kipiplugin_imgurexport") )

class Plugin_ImgurExport::Private
{
public:

    Private()
    {
        actionExport = 0;
        winExport    = 0;
    }

    KAction*     actionExport;
    ImgurWindow* winExport;
};

Plugin_ImgurExport::Plugin_ImgurExport(QObject* const parent, const QVariantList& args)
    : Plugin(ImgurExportFactory::componentData(), parent, "ImgurExport"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "ImgurExport plugin loaded";
    kDebug(AREA_CODE_LOADING) << args;

    KIconLoader::global()->addAppDir("kipiplugin_imgurexport");

    setUiBaseName("kipiplugin_imgurexportui.rc");
    setupXML();
}

Plugin_ImgurExport::~Plugin_ImgurExport()
{
    delete d;
}

void Plugin_ImgurExport::setup(QWidget* const widget)
{
    d->winExport = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_ImgurExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new KAction(this);
    d->actionExport->setText(i18n("Export to &Imgur..."));
    d->actionExport->setIcon(KIcon("kipi-imgur"));

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("imgurexport", d->actionExport);
}

void Plugin_ImgurExport::slotActivate()
{
    if (!d->winExport)
    {
        // We clean it up in the close button
        d->winExport = new ImgurWindow(kapp->activeWindow());
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

    kDebug() << "We have activated the imgur exporter!";
}

} // namespace KIPIImgurExportPlugin
