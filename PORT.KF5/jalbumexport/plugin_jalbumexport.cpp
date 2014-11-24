/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : A KIPI plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#include "plugin_jalbumexport.moc"

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

#include "jalbum.h"
#include "jalbumconfig.h"
#include "jalbumwindow.h"

namespace KIPIJAlbumExportPlugin
{

K_PLUGIN_FACTORY(JAlbumExportFactory, registerPlugin<Plugin_JAlbumExport>();)
K_EXPORT_PLUGIN(JAlbumExportFactory("kipiplugin_jalbumexport") )

class Plugin_JAlbumExport::Private
{
public:

    Private()
    {
        actionExport = 0;
        jalbum       = 0;
    }

    JAlbum*  jalbum;
    KAction* actionExport;
};

Plugin_JAlbumExport::Plugin_JAlbumExport(QObject* const parent, const QVariantList&)
    : Plugin(JAlbumExportFactory::componentData(), parent, "JAlbumExport"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_JAlbumExport plugin loaded";

    d->jalbum = new JAlbum();
    setUiBaseName("kipiplugin_jalbumexportui.rc");
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
        kError() << "KIPI interface is null!";
        return;
    }

    KIconLoader::global()->addAppDir("kipiplugin_jalbumexport");
    setupActions();
}

void Plugin_JAlbumExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new KAction(this);
    d->actionExport->setText(i18n("Export via &jAlbum"));
    d->actionExport->setIcon(KIcon("kipi-jalbum"));

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction("jalbumexport", d->actionExport);
}

void Plugin_JAlbumExport::slotExport()
{
    QPointer<JAlbumEdit>   configDlg;
    QPointer<JAlbumWindow> dlg;

    KConfig config("kipirc");

    if(!config.hasGroup("jAlbum Settings") )
    {
        configDlg = new JAlbumEdit(kapp->activeWindow(), d->jalbum, i18n("Edit jAlbum Data") );
        configDlg->exec();
    }

    dlg = new JAlbumWindow(kapp->activeWindow(), d->jalbum);
    dlg->exec();

    delete configDlg;
    delete dlg;
}

}  // namespace KIPIJAlbumExportPlugin
