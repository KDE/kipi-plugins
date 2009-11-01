/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz@kde.org>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_ipodexport.h"
#include "plugin_ipodexport.moc"

extern "C"
{
#include <glib-object.h> // g_type_init
}

// KDE includes

#include <kactioncollection.h>
#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kwindowsystem.h>

// LibKipi includes

#include <libkipi/imagecollection.h>

// Local includes

#include "IpodExportDialog.h"

using namespace KIPIIpodExportPlugin;

K_PLUGIN_FACTORY( IpodFactory, registerPlugin<Plugin_iPodExport>(); )
K_EXPORT_PLUGIN ( IpodFactory("kipiplugin_ipodexport") )

Plugin_iPodExport::Plugin_iPodExport( QObject *parent, const QVariantList& )
                 : KIPI::Plugin( IpodFactory::componentData(), parent, "iPodExport")
{
    m_dlgImageUpload = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_iPodExport plugin loaded" ;

    g_type_init();
}

void Plugin_iPodExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_actionImageUpload = actionCollection()->addAction("ipodexport");
    m_actionImageUpload->setText(i18n("Export to &iPod..."));
    m_actionImageUpload->setIcon(KIcon("multimedia-player-apple-ipod"));
    m_actionImageUpload->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_I));

    connect(m_actionImageUpload, SIGNAL(triggered(bool)),
            this, SLOT(slotImageUpload()));

    addAction(m_actionImageUpload);

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
}

void Plugin_iPodExport::slotImageUpload()
{
    if (!m_dlgImageUpload)
    {
        m_dlgImageUpload = new UploadDialog(m_interface, i18n("iPod Export"), kapp->activeWindow());
    }
    else
    {
        if (m_dlgImageUpload->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImageUpload->winId());

        KWindowSystem::activateWindow(m_dlgImageUpload->winId());
    }

    m_dlgImageUpload->setMinimumWidth(650);
    m_dlgImageUpload->reactivate();
}

KIPI::Category Plugin_iPodExport::category(KAction* action) const
{
    if ( action == m_actionImageUpload )
        return KIPI::ExportPlugin;

    return KIPI::ImagesPlugin; // no warning from compiler, please
}
