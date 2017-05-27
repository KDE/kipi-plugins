/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : A KIPI plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013-2017 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

// Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>
#include <kconfig.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "jalbum.h"
#include "jalbumconfig.h"
#include "jalbumwindow.h"
#include "plugin_jalbum.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPIJAlbumExportPlugin
{

K_PLUGIN_FACTORY(JAlbumFactory, registerPlugin<Plugin_JAlbumExport>();)

class Plugin_JAlbumExport::Private
{
public:

    Private()
    {
        actionExport = 0;
        jalbum       = 0;
    }

    JAlbum*  jalbum;
    QAction* actionExport;
};

Plugin_JAlbumExport::Plugin_JAlbumExport(QObject* const parent, const QVariantList&)
    : Plugin(parent, "JAlbum"),
      d(new Private)
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_JAlbum plugin loaded";

    d->jalbum = new JAlbum();
    setUiBaseName("kipiplugin_jalbumui.rc");
    setupXML();
}

Plugin_JAlbumExport::~Plugin_JAlbumExport()
{
    delete d->jalbum;
    delete d;
}

void Plugin_JAlbumExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_JAlbumExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new QAction(this);
    d->actionExport->setText(i18n("Export to &jAlbum"));
    d->actionExport->setIcon(QIcon::fromTheme(QLatin1String("kipi-jalbum")));

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QLatin1String("jalbum"), d->actionExport);
}

void Plugin_JAlbumExport::slotExport()
{
    QPointer<JAlbumEdit>   configDlg;
    QPointer<JAlbumWindow> dlg;

    KConfig config(QLatin1String("kipirc"));

    if (!config.hasGroup(QLatin1String("jAlbum Settings")))
    {
        configDlg = new JAlbumEdit(QApplication::activeWindow(), d->jalbum, i18n("Edit jAlbum Data") );
        configDlg->exec();
    }

    dlg = new JAlbumWindow(QApplication::activeWindow(), d->jalbum);
    dlg->exec();

    delete configDlg;
    delete dlg;
}

}  // namespace KIPIJAlbumExportPlugin

#include "plugin_jalbum.moc"
