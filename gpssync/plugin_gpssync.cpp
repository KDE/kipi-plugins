/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with
 *               a GPS device.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_gpssync.h"
#include "plugin_gpssync.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "gpssyncdialog.h"

K_PLUGIN_FACTORY( GPSSyncFactory, registerPlugin<Plugin_GPSSync>(); )
K_EXPORT_PLUGIN ( GPSSyncFactory("kipiplugin_gpssync2") )

Plugin_GPSSync::Plugin_GPSSync(QObject *parent, const QVariantList &)
              : KIPI::Plugin( GPSSyncFactory::componentData(), parent, "GPSSync2")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_GPSSync2 plugin loaded" ;
}

void Plugin_GPSSync::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_geolocation = new KActionMenu(KIcon("applications-internet"), i18n("Geolocation2"), actionCollection());

    KAction *gpssync = actionCollection()->addAction("gpssync2");
    gpssync->setText(i18n("Correlator2"));
    gpssync->setIcon(KIcon("gpsimagetag"));
    connect(gpssync, SIGNAL(triggered(bool)),
            this, SLOT(slotGPSSync()));
    m_action_geolocation->addAction(gpssync);

    addAction( m_action_geolocation );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_action_geolocation->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect( m_interface, SIGNAL(selectionChanged(bool)),
             m_action_geolocation, SLOT(setEnabled(bool)));
}

void Plugin_GPSSync::slotGPSSync()
{
    kDebug()<<1;
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    /* NOTE: this plugin do not use yet GPSBabel to convert GPS data file to GPX
    QString gpsBabelVersion;
    if (!checkBinaries(gpsBabelVersion))
        return;
    */

    KIPIGPSSyncPlugin::GPSSyncDialog *dialog = new KIPIGPSSyncPlugin::GPSSyncDialog(
                                               m_interface, kapp->activeWindow());

    dialog->setImages( images.images() );
    dialog->show();
}

KIPI::Category Plugin_GPSSync::category( KAction* action ) const
{
    if ( action == m_action_geolocation )
       return KIPI::ImagesPlugin;
    if ( action == m_actionKMLExport )
       return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
