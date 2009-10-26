/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_galleryexport.h"
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

K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_GalleryExport>();)
K_EXPORT_PLUGIN(Factory("kipiplugin_galleryexport"))

Plugin_GalleryExport::Plugin_GalleryExport(QObject *parent, const QVariantList&)
                    : KIPI::Plugin(Factory::componentData(), parent, "GalleryExport"),
                      m_action(0), mpGallery(0)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_GalleryExport plugin loaded";
}

void Plugin_GalleryExport::setup(QWidget* widget)
{
    KIconLoader::global()->addAppDir("kipiplugin_galleryexport");

    mpGallery = new KIPIGalleryExportPlugin::Gallery();

    KIPI::Plugin::setup(widget);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_action = actionCollection()->addAction("galleryexport");
    m_action->setText(i18n("Export to &Gallery..."));
    m_action->setIcon(KIcon("gallery"));
    m_action->setEnabled(true);
    m_action->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_G));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotSync()));

    addAction(m_action);
}

Plugin_GalleryExport::~Plugin_GalleryExport()
{
    if (mpGallery)
        delete mpGallery;
}

// this slot uses GalleryWindow Class
void Plugin_GalleryExport::slotSync()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    QPointer<KIPIGalleryExportPlugin::GalleryEdit>   configDlg;
    QPointer<KIPIGalleryExportPlugin::GalleryWindow> dlg;

    KConfig config("kipirc");
    if(!config.hasGroup("Gallery Settings") )
    {
        configDlg = new KIPIGalleryExportPlugin::GalleryEdit(kapp->activeWindow(),
                                                             mpGallery, i18n("Edit Gallery Data") );
        configDlg->exec();
    }

    dlg = new KIPIGalleryExportPlugin::GalleryWindow(interface, kapp->activeWindow(), mpGallery);
    dlg->exec();

    delete configDlg;
    delete dlg;
}

KIPI::Category Plugin_GalleryExport::category(KAction* action) const
{
    if (action == m_action)
        return KIPI::ExportPlugin;
//     if (action == m_action_configure)
//         return KIPI::ToolsPlugin;
//
    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
