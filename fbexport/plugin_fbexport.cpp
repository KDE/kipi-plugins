/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#include "plugin_fbexport.h"
#include "plugin_fbexport.moc"

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
#include "fbwindow.h"

K_PLUGIN_FACTORY( FbExportFactory, registerPlugin<Plugin_FbExport>(); )
K_EXPORT_PLUGIN ( FbExportFactory("kipiplugin_fbexport") )

Plugin_FbExport::Plugin_FbExport(QObject *parent,
                                     const QVariantList &/*args*/)
                   : KIPI::Plugin(FbExportFactory::componentData(),
                                  parent, "FbExport")
{
    kDebug(51001) << "Plugin_FbExport plugin loaded";
}

void Plugin_FbExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action = actionCollection()->addAction("fbexport");
    m_action->setText(i18n("Export to Facebook..."));
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

Plugin_FbExport::~Plugin_FbExport()
{
}

void Plugin_FbExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-fbexport-"
                                           + QString::number(getpid()) + '/');

    // We clean it up in the close button
    m_dlg = new KIPIFbExportPlugin::FbWindow(interface, tmp,
                                                 kapp->activeWindow());
    m_dlg->show();
}

KIPI::Category Plugin_FbExport::category( KAction* action ) const
{
    if (action == m_action)
        return KIPI::ExportPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
