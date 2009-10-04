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
    kDebug(51001) << "Plugin_KioExportImport plugin loaded";
}

void Plugin_KioExportImport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    // export
    m_actionExport = actionCollection()->addAction("kioexport");
    m_actionExport->setText(i18n("Export to remote computer..."));
    m_actionExport->setIcon(KIcon("folder-remote"));
    m_actionExport->setShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_K);

    connect(m_actionExport, SIGNAL(triggered(bool)), 
            this, SLOT(slotActivateExport()));

    addAction(m_actionExport);

    // import
    m_actionImport = actionCollection()->addAction("kioimport");
    m_actionImport->setText(i18n("Import from remote computer..."));
    m_actionImport->setIcon(KIcon("folder-remote"));
    m_actionImport->setShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_I);

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateImport()));

    addAction(m_actionImport);

    // check interface availability
    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError(51000) << "KIPI::Interface empty";
        m_actionExport->setEnabled(false);
        m_actionImport->setEnabled(false);
        return;
    }
}

void Plugin_KioExportImport::slotActivateExport()
{
    kDebug(51000) << "Starting KIO export";

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError(51000) << "KIPI::Interface empty";
        return;
    }

    KioExportWindow *window = new KioExportWindow(kapp->activeWindow(), interface);
    window->show();
}

void Plugin_KioExportImport::slotActivateImport()
{
    kDebug(51000) << "Starting KIO import";

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError(51000) << "KIPI::Interface empty";
        return;
    }

    KioImportWindow *window = new KioImportWindow(kapp->activeWindow(), interface);
    window->show();
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
        kWarning(51000) << "Received unknown action";
        return KIPI::ExportPlugin;
    }
}
