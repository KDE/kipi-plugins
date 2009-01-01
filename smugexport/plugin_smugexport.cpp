/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#include "plugin_smugexport.h"
#include "plugin_smugexport.moc"

// C ANSI includes.
extern "C"
{
#include <unistd.h>
}

// KDE includes.
#include <kdebug.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kstandarddirs.h>

// LibKIPI includes.
#include <libkipi/interface.h>

// Local includes.
#include "smugwindow.h"

K_PLUGIN_FACTORY( SmugExportFactory, registerPlugin<Plugin_SmugExport>(); )
K_EXPORT_PLUGIN ( SmugExportFactory("kipiplugin_smugexport") )

Plugin_SmugExport::Plugin_SmugExport(QObject *parent,
                                     const QVariantList &/*args*/)
                   : KIPI::Plugin(SmugExportFactory::componentData(),
                                  parent, "SmugExport")
{
    kDebug(51001) << "Plugin_SmugExport plugin loaded";
}

void Plugin_SmugExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action = actionCollection()->addAction("smugexport");
    m_action->setText(i18n("Export to SmugMug..."));
    m_action->setIcon(KIcon("applications-internet"));

    connect(m_action, SIGNAL( triggered(bool) ),
            this, SLOT( slotActivate()) );

    addAction(m_action);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!";
        m_action->setEnabled(false);
        return;
    }

    m_action->setEnabled(true);
}

Plugin_SmugExport::~Plugin_SmugExport()
{
}

void Plugin_SmugExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-smugexport-"
                                           + QString::number(getpid()) + "/");

    // We clean it up in the close button
    m_dlg = new KIPISmugExportPlugin::SmugWindow(interface, tmp,
                                                 kapp->activeWindow());
    m_dlg->show();
}

KIPI::Category Plugin_SmugExport::category( KAction* action ) const
{
    if (action == m_action)
        return KIPI::ExportPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
