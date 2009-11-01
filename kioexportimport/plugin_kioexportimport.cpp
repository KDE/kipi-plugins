/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export image to a KIO accessible
 *               location
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
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

// Local includes

#include "plugin_kioexportimport.h"
#include "plugin_kioexportimport.moc"

// KDE includes

#include <kactioncollection.h>
#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kwindowsystem.h>

// LibKipi includes

#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "KioExportWindow.h"
#include "KioImportWindow.h"

using namespace KIPIKioExportPlugin;

K_PLUGIN_FACTORY( KioFactory, registerPlugin<Plugin_KioExportImport>(); )
K_EXPORT_PLUGIN ( KioFactory("kipiplugin_kioexportimport") )

Plugin_KioExportImport::Plugin_KioExportImport(QObject *parent, const QVariantList&)
                      : KIPI::Plugin(KioFactory::componentData(), parent, "KioExportImport")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_KioExportImport plugin loaded";
}

void Plugin_KioExportImport::setup(QWidget* widget)
{
    m_dlgExport = 0;
    m_dlgImport = 0;

    KIPI::Plugin::setup(widget);

    // export
    m_actionExport = actionCollection()->addAction("kioexport");
    m_actionExport->setText(i18n("Export to remote computer..."));
    m_actionExport->setIcon(KIcon("folder-remote"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_K));

    connect(m_actionExport, SIGNAL(triggered(bool)), 
            this, SLOT(slotActivateExport()));

    addAction(m_actionExport);

    // import
    m_actionImport = actionCollection()->addAction("kioimport");
    m_actionImport->setText(i18n("Import from remote computer..."));
    m_actionImport->setIcon(KIcon("folder-remote"));
    m_actionImport->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_I));

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateImport()));

    addAction(m_actionImport);

    // check interface availability
    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError() << "KIPI::Interface empty";
        m_actionExport->setEnabled(false);
        m_actionImport->setEnabled(false);
        return;
    }
}

void Plugin_KioExportImport::slotActivateExport()
{
    kDebug() << "Starting KIO export";

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError() << "KIPI::Interface empty";
        return;
    }

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KioExportWindow(kapp->activeWindow(), interface);
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_KioExportImport::slotActivateImport()
{
    kDebug() << "Starting KIO import";

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError() << "KIPI::Interface empty";
        return;
    }

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new KioImportWindow(kapp->activeWindow(), interface);
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

KIPI::Category Plugin_KioExportImport::category(KAction* action) const
{
    if (action == m_actionExport)
    {
        return KIPI::ExportPlugin;
    }
    else if (action == m_actionImport)
    {
        return KIPI::ImportPlugin;
    }
    else
    {
        kWarning() << "Received unknown action";
        return KIPI::ExportPlugin;
    }
}
