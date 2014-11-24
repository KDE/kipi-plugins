/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : A KIPI plugin to export with DLNA Technology
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "plugin_dlnaexport.moc"

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kaction.h>
#include <klibloader.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kshortcut.h>
#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>
#include <kapplication.h>

// Libkipi includes

#include <libkipi/interface.h>

// Local includes

#include "wizard.h"

namespace KIPIDLNAExportPlugin
{

K_PLUGIN_FACTORY(DLNAExportFactory, registerPlugin<Plugin_DLNAExport>();)
K_EXPORT_PLUGIN(DLNAExportFactory("kipiplugin_dlnaexport") )

class Plugin_DLNAExport::Private
{
public:

    Private()
    {
        actionExport = 0;
        dlgExport    = 0;
    }

    KAction* actionExport;
    Wizard*  dlgExport;
};

Plugin_DLNAExport::Plugin_DLNAExport(QObject* const parent, const QVariantList&)
    : Plugin(DLNAExportFactory::componentData(), parent, "DLNAExport"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_DLNAExport plugin loaded";

    setUiBaseName("kipiplugin_dlnaexportui.rc");
    setupXML();
}

Plugin_DLNAExport::~Plugin_DLNAExport()
{
    delete d;
}

void Plugin_DLNAExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    if (!interface())
    {
        kError() << "KIPI interface is null!";
        return;
    }

    KIconLoader::global()->addAppDir("kipiplugin_dlnaexport");
    setupActions();
}

void Plugin_DLNAExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new KAction(this);
    d->actionExport->setText(i18n("Export via &DLNA"));
    d->actionExport->setIcon(KIcon("kipi-dlna"));

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction("dlnaexport", d->actionExport);
}

void Plugin_DLNAExport::slotExport()
{
    if (!d->dlgExport)
    {
        // We clean it up in the close button
        d->dlgExport = new Wizard(kapp->activeWindow());
    }
    else
    {
        if (d->dlgExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(d->dlgExport->winId());
            KWindowSystem::activateWindow(d->dlgExport->winId());
        }
        else
        {
            delete d->dlgExport;
            d->dlgExport = new Wizard(kapp->activeWindow());
        }
    }

    d->dlgExport->show();
}

}  // namespace KIPIDLNAExportPlugin
