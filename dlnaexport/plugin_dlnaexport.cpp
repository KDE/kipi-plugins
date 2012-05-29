/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : A KIPI plugin to export with DLNA Technology
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "plugin_dlnaexport.moc"

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

namespace KIPIDLNAExportPlugin
{

K_PLUGIN_FACTORY(DLNAExportFactory, registerPlugin<Plugin_DLNAExport>();)
K_EXPORT_PLUGIN(DLNAExportFactory("kipiplugin_dlnaexport") )

Plugin_DLNAExport::Plugin_DLNAExport(QObject* const parent, const QVariantList&)
    : Plugin(DLNAExportFactory::componentData(), parent, "DLNAExport"),
    m_actionExport(0),
    m_dlgExport(0)
{
}

Plugin_DLNAExport::~Plugin_DLNAExport()
{
}

void Plugin_DLNAExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_dlnaexport");

    m_actionExport = actionCollection()->addAction("dlnaexport");
    m_actionExport->setText(i18n("Export via &DLNA"));
    m_actionExport->setIcon(KIcon("dlna"));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(m_actionExport);

    m_actionExport->setEnabled(true);
}

void Plugin_DLNAExport::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-dlnaexportplugin-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new DLNAWindow(tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }
}

KIPI::Category Plugin_DLNAExport::category(KAction* const action) const
{
    if (action == m_actionExport)
       return ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin;
}

}  // namespace KIPIDLNAExportPlugin
