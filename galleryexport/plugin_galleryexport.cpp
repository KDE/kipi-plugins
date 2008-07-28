/* ============================================================
 * File  : plugin_galleryexport.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Modified By : Colin Guthrie
 * Date  : 2004-11-06
 * Description :
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2006 by Colin Guthrie <kde@colin.guthr.ie>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// KDE includes.
#include <KLocale>
#include <KAction>
#include <KDebug>
#include <KMessageBox>
#include <KIconLoader>
#include <KApplication>
#include <KGenericFactory>

// libkipi includes.
#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// Local includes.
#include "galleries.h"
#include "gallerylist.h"
#include "gallerywindow.h"
#include "galleryconfig.h"
#include "plugin_galleryexport.h"


K_PLUGIN_FACTORY(Factory, registerPlugin<Plugin_GalleryExport>();)
K_EXPORT_PLUGIN(Factory("kipiplugin_galleryexport"))


Plugin_GalleryExport::Plugin_GalleryExport(QObject *parent,
        const QVariantList&)
        : KIPI::Plugin(Factory::componentData(), parent, "GalleryExport"),
        mpGalleries(0)
{
    kDebug(51001) << "Plugin_GalleryExport plugin loaded"
    << endl;
}

void Plugin_GalleryExport::setup(QWidget* widget)
{
    mpGalleries = new KIPIGalleryExportPlugin::Galleries();

    KIPI::Plugin::setup(widget);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface) {
        kError(51000) << "Kipi interface is null!" << endl;
        return;
    }

    m_action_sync = new KAction(i18n("remote-gallery-sync"), this);
    m_action_sync->setText(i18n("Gallery Export..."));
    m_action_sync->setIcon(KIcon("applications-internet"));
    connect(m_action_sync, SIGNAL(triggered(bool)), this, SLOT(slotSync()));
    m_action_sync->setEnabled(true);
    addAction(m_action_sync);


    m_action_configure = new KAction(i18n("remote-galleries-setting"), this);
    m_action_configure->setText("Remote Gallery Setting...");
    m_action_configure->setIcon(KIcon("applications-system"));
    connect(m_action_configure, SIGNAL(triggered(bool)), this, SLOT(slotConfigure()));
    m_action_configure->setEnabled(true);
    addAction(m_action_configure);


//     m_action_collection_settings = new KAction( i18n("remote-gallery-collection-settings"), this );
//     m_action_collection_settings->setText("Remote Gallery Collection Settings...");
//     connect( m_action_collection_settings, SIGNAL(triggered(bool)), this, SLOT(slotCollectionSettings()) );
//     m_action_collection_settings->setEnabled(true);
//     addAction(m_action_collection_settings);


//     m_action_image_setting = new KAction(i18n("remote-gallery-image-settings"), this );
//     m_action_image_setting->setText("Remote Gallery Image Settings...");
//     connect( m_action_image_setting, SIGNAL(triggered(bool)), this, SLOT(slotImageSettings()) );
//     m_action_image_setting->setEnabled(true);
//     addAction(m_action_image_setting);

}


Plugin_GalleryExport::~Plugin_GalleryExport()
{
    if (mpGalleries)
        delete mpGalleries;
}

// this slot uses GalleryWindow Class
void Plugin_GalleryExport::slotSync()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface) {
        kError(51000) << "Kipi interface is null!" << endl;
        return;
    }

    KIPIGalleryExportPlugin::GalleryWindow dlg(interface, kapp->activeWindow(), mpGalleries);
    dlg.exec();
}

// this slot uses GalleryList Class
void Plugin_GalleryExport::slotConfigure()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface) {
        kError(51000) << "Kipi interface is null!" << endl;
        return;
    }

    KIPIGalleryExportPlugin::GalleryList dlg(kapp->activeWindow(), mpGalleries, false);
    dlg.exec();
}


// void Plugin_GalleryExport::slotCollectionSettings()
// {
//     KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
//     if (!interface)
//     {
//         kError( 51000 ) << "Kipi interface is null!" << endl;
//         return;
//     }
//
//     KMessageBox::error(kapp->activeWindow(), "Not Implemented Yet!");
// }

// void Plugin_GalleryExport::slotImageSettings()
// {
//     KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
//     if (!interface)
//     {
//         kError( 51000 ) << "Kipi interface is null!" << endl;
//         return;
//     }
//
//     KMessageBox::error(kapp->activeWindow(), "Not Implemented Yet!");
// }

KIPI::Category Plugin_GalleryExport::category(KAction* action) const
{
    if (action == m_action_sync)
        return KIPI::ExportPlugin;
    if (action == m_action_configure)
        return KIPI::ToolsPlugin;
//     if (action == m_action_collection_settings)
//         return KIPI::CollectionsPlugin;
//     if (action == m_action_image_setting)
//         return KIPI::ImagesPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification"
    << endl;
    return KIPI::ExportPlugin;
}


#include "plugin_galleryexport.moc"
