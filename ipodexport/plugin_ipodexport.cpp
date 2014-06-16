/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz at kde dot org>
 * Copyright (C) 2008-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "uploaddialog.h"

namespace KIPIIpodExportPlugin
{

K_PLUGIN_FACTORY( IpodFactory, registerPlugin<Plugin_iPodExport>(); )
K_EXPORT_PLUGIN ( IpodFactory("kipiplugin_ipodexport") )

Plugin_iPodExport::Plugin_iPodExport(QObject* const parent, const QVariantList&)
    : Plugin( IpodFactory::componentData(), parent, "iPodExport")
{
    m_dlgImageUpload = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_iPodExport plugin loaded" ;

#if !defined(GLIB_VERSION_2_36)
    g_type_init();
#endif

    setUiBaseName("kipiplugin_ipodexportui.rc");
    setupXML();
}

Plugin_iPodExport::~Plugin_iPodExport()
{
}

void Plugin_iPodExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);
    setupActions();
}

void Plugin_iPodExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionImageUpload = new KAction(this);
    m_actionImageUpload->setText(i18n("Export to &iPod..."));
    m_actionImageUpload->setIcon(KIcon("multimedia-player-apple-ipod"));

    connect(m_actionImageUpload, SIGNAL(triggered(bool)),
            this, SLOT(slotImageUpload()));

    addAction("ipodexport", m_actionImageUpload);
}

void Plugin_iPodExport::slotImageUpload()
{
    if (!m_dlgImageUpload)
    {
        m_dlgImageUpload = new UploadDialog(i18n("iPod Export"), kapp->activeWindow());
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

} // namespace KIPIIpodExportPlugin
