/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// To disable warnings under MSVC2008 about getpid().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_wikimedia.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kdebug.h>
#include <KConfig>
#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KGenericFactory>
#include <KLibLoader>
#include <KStandardDirs>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "wmwindow.h"

namespace KIPIWikiMediaPlugin
{

K_PLUGIN_FACTORY( WikiMediaFactory, registerPlugin<Plugin_WikiMedia>(); )
K_EXPORT_PLUGIN ( WikiMediaFactory("kipiplugin_wikimedia") )

class Plugin_WikiMedia::Private
{
public:

    Private()
    {
        actionExport = 0;
        dlgExport    = 0;
    }

    KAction*  actionExport;
    WMWindow* dlgExport;
};

Plugin_WikiMedia::Plugin_WikiMedia(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(WikiMediaFactory::componentData(), parent, "MediaWiki export"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_MediaWiki plugin loaded";

    setUiBaseName("kipiplugin_wikimediaui.rc");
    setupXML();
}

Plugin_WikiMedia::~Plugin_WikiMedia()
{
    delete d;
}

void Plugin_WikiMedia::setup(QWidget* const widget)
{
    d->dlgExport = 0;
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_wikimedia");

    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    d->actionExport->setEnabled(true);
}

void Plugin_WikiMedia::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new KAction(this);
    d->actionExport->setText(i18n("Export to MediaWiki..."));
    d->actionExport->setIcon(KIcon("kipi-wikimedia"));
    d->actionExport->setEnabled(false);

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction("wikimediaexport", d->actionExport);
}

void Plugin_WikiMedia::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-mediawiki-") + QString::number(getpid()) + QString("/"));

    if (!d->dlgExport)
    {
        // We clean it up in the close button
        d->dlgExport = new WMWindow(tmp, kapp->activeWindow());
    }
    else
    {
        if (d->dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(d->dlgExport->winId());

        d->dlgExport->reactivate();
    }
}

} // namespace KIPIWikiMediaPlugin
