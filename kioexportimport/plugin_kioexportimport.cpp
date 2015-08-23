/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "KioExportWindow.h"
#include "KioImportWindow.h"

namespace KIPIKioExportPlugin
{

K_PLUGIN_FACTORY( KioFactory, registerPlugin<Plugin_KioExportImport>(); )
K_EXPORT_PLUGIN ( KioFactory("kipiplugin_kioexportimport") )

Plugin_KioExportImport::Plugin_KioExportImport(QObject* const parent, const QVariantList&)
    : Plugin(KioFactory::componentData(), parent, "KioExportImport")
{
    m_actionExport = 0;
    m_actionImport = 0;
    m_dlgExport    = 0;
    m_dlgImport    = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_KioExportImport plugin loaded";

    setUiBaseName("kipiplugin_kioexportimportui.rc");
    setupXML();
}

Plugin_KioExportImport::~Plugin_KioExportImport()
{
}

void Plugin_KioExportImport::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
    m_actionImport->setEnabled(true);
}

void Plugin_KioExportImport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to remote computer..."));
    m_actionExport->setIcon(KIcon("folder-remote"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_K));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateExport()));

    addAction("kioexport", m_actionExport);

    // import
    m_actionImport = new KAction(this);
    m_actionImport->setText(i18n("Import from remote computer..."));
    m_actionImport->setIcon(KIcon("folder-remote"));
    m_actionImport->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_I));
    m_actionImport->setEnabled(false);

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateImport()));

    addAction("kioimport", m_actionImport, ImportPlugin);
}

void Plugin_KioExportImport::slotActivateExport()
{
    kDebug() << "Starting KIO export";

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KioExportWindow(kapp->activeWindow());
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

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new KioImportWindow(kapp->activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

} // namespace KIPIKioExportPlugin
