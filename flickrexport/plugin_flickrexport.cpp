/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C ANSI includes.

extern "C"
{
#include <unistd.h>
}

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "flickrwindow.h"
#include "plugin_flickrexport.h"
#include "plugin_flickrexport.moc"

K_PLUGIN_FACTORY( FlickrExportFactory, registerPlugin<Plugin_FlickrExport>(); )
K_EXPORT_PLUGIN ( FlickrExportFactory("kipiplugin_flickrexport") )

Plugin_FlickrExport::Plugin_FlickrExport(QObject *parent, const QVariantList &/*args*/)
                   : KIPI::Plugin(FlickrExportFactory::componentData(), parent, "FlickrExport")
{
    kDebug(51001) << "Plugin_FlickrExport plugin loaded" << endl;
}

void Plugin_FlickrExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action = new KAction(KIcon("applications-internet"), i18n("Export to Flickr..."), actionCollection());
    m_action->setObjectName("flickrexport");
    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        m_action->setEnabled(false);
        return;
    }

    m_action->setEnabled(true);
    addAction(m_action);
}

Plugin_FlickrExport::~Plugin_FlickrExport()
{
}

void Plugin_FlickrExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-flickrexportplugin-" + QString::number(getpid()) + "/");

    // We clean it up in the close button
    m_dlg = new KIPIFlickrExportPlugin::FlickrWindow(interface, tmp, kapp->activeWindow());
    m_dlg->show();
}

KIPI::Category Plugin_FlickrExport::category( KAction* action ) const
{
    if (action == m_action)
        return KIPI::ExportPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::ExportPlugin;
}
