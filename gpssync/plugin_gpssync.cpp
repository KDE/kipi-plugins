/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-05-16
 * @brief  A plugin to synchronize pictures with a GPS device.
 *
 * @author Copyright (C) 2006-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

// Local includes

#include "gpssyncdialog.h"

namespace KIPIGPSSyncPlugin
{

K_PLUGIN_FACTORY( GPSSyncFactory, registerPlugin<Plugin_GPSSync>(); )
K_EXPORT_PLUGIN ( GPSSyncFactory("kipiplugin_gpssync") )

Plugin_GPSSync::Plugin_GPSSync(QObject* const parent, const QVariantList&)
    : Plugin( GPSSyncFactory::componentData(), parent, "GPSSync")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_GPSSync plugin loaded" ;
}

void Plugin_GPSSync::setup(QWidget* widget)
{
    Plugin::setup( widget );

    m_action_geolocation = actionCollection()->addAction("gpssync");
    m_action_geolocation->setText(i18n("Geo-location"));
    m_action_geolocation->setIcon(KIcon("applications-internet"));

    connect(m_action_geolocation, SIGNAL(triggered(bool)),
            this, SLOT(slotGPSSync()));

    addAction(m_action_geolocation);

    m_interface = dynamic_cast< Interface* >( parent() );

    if ( !m_interface )
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    ImageCollection selection = m_interface->currentSelection();
    m_action_geolocation->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect(m_interface, SIGNAL(selectionChanged(bool)),
            m_action_geolocation, SLOT(setEnabled(bool)));
}

void Plugin_GPSSync::slotGPSSync()
{
    ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    GPSSyncDialog* dialog = new GPSSyncDialog(kapp->activeWindow());

    dialog->setImages( images.images() );
    dialog->show();
}

Category Plugin_GPSSync::category( KAction* action ) const
{
    if ( action == m_action_geolocation )
       return ImagesPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return ImagesPlugin; // no warning from compiler, please
}

} // namespace KIPIGPSSyncPlugin