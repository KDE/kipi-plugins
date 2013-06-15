/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "plugin_galleryexport.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// Local includes

#include "galleries.h"
#include "galleryconfig.h"
#include "gallerywindow.h"

namespace KIPIGalleryExportPlugin
{

K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_GalleryExport>();)
K_EXPORT_PLUGIN(Factory("kipiplugin_galleryexport"))

class Plugin_GalleryExport::Private
{
public:

    Private()
    {
        action  = 0;
        gallery = 0;
    }

    KAction* action;
    Gallery* gallery;
};

Plugin_GalleryExport::Plugin_GalleryExport(QObject* const parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "GalleryExport"),
      d(new Private())
{
    kDebug(AREA_CODE_LOADING) << "Plugin_GalleryExport plugin loaded";

    KIconLoader::global()->addAppDir("kipiplugin_galleryexport");
    d->gallery = new Gallery();

    setUiBaseName("kipiplugin_galleryexportui.rc");
    setupXML();
}

Plugin_GalleryExport::~Plugin_GalleryExport()
{
    delete d->gallery;
    delete d;
}

void Plugin_GalleryExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_GalleryExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->action = new KAction(this);
    d->action->setText(i18n("Export to &Gallery..."));
    d->action->setIcon(KIcon("kipi-gallery"));
    d->action->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_G));

    connect(d->action, SIGNAL(triggered(bool)),
            this, SLOT(slotSync()));

    addAction("galleryexport", d->action);
}

// this slot uses GalleryWindow Class
void Plugin_GalleryExport::slotSync()
{
    QPointer<GalleryEdit>   configDlg;
    QPointer<GalleryWindow> dlg;

    KConfig config("kipirc");

    if(!config.hasGroup("Gallery Settings") )
    {
        configDlg = new GalleryEdit(kapp->activeWindow(), d->gallery, i18n("Edit Gallery Data") );
        configDlg->exec();
    }

    dlg = new GalleryWindow(kapp->activeWindow(), d->gallery);
    dlg->exec();

    delete configDlg;
    delete dlg;
}

} // namespace KIPIGalleryExportPlugin
