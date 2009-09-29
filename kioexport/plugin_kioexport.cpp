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

#include "plugin_kioexport.h"

// KDE includes

#include <kactioncollection.h>
#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>

// LibKipi includes

#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "KioExportWindow.h"

using namespace KIPIKioExportPlugin;

K_PLUGIN_FACTORY( KioFactory, registerPlugin<Plugin_KioExport>(); )
K_EXPORT_PLUGIN ( KioFactory("kipiplugin_kioexport") )

Plugin_KioExport::Plugin_KioExport(QObject *parent, const QVariantList&)
                : KIPI::Plugin(KioFactory::componentData(), parent, "KioExport")
{
    kDebug(51001) << "Plugin_KioExport plugin loaded";
}

void Plugin_KioExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action = actionCollection()->addAction("kioexport");
    m_action->setText(i18n("Export using &KIO..."));
    m_action->setIcon(KIcon("system-file-manager"));
    m_action->setShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_K);

    connect(m_action, SIGNAL(triggered(bool)), 
            this, SLOT(slotActivate()));

    addAction(m_action);

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError(51000) << "KIPI::Interface empty";
        m_action->setEnabled(false);
        return;
    }
}

void Plugin_KioExport::slotActivate()
{
    kDebug(51000) << "Plugin_KioExport::slotImageUpload called";

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*> (parent());
    if (!interface)
    {
        kError(51000) << "KIPI::Interface empty";
        return;
    }

    KioExportWindow *window = new KioExportWindow(kapp->activeWindow(), interface);
    window->show();
}

KIPI::Category Plugin_KioExport::category(KAction* action) const
{
    if (action == m_action)
    {
        return KIPI::ExportPlugin;
    }
    else
    {
        kWarning(51000) << "Received unknown action";
        return KIPI::ExportPlugin;
    }
}
