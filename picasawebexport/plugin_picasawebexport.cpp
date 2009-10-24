/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-17-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#include "plugin_picasawebexport.h"
#include "plugin_picasawebexport.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "picasawebwindow.h"

K_PLUGIN_FACTORY( PicasawebExportFactory, registerPlugin<Plugin_PicasawebExport>(); )
K_EXPORT_PLUGIN ( PicasawebExportFactory("kipiplugin_picasawebexport") )

Plugin_PicasawebExport::Plugin_PicasawebExport(QObject *parent, const QVariantList&)
                      : KIPI::Plugin(PicasawebExportFactory::componentData(), parent, "PicasawebExport")
{
    m_dlgExport = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_PicasawebExport plugin loaded" ;
}

void Plugin_PicasawebExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_picasawebexport");

    m_actionExport = actionCollection()->addAction("picasawebexport");
    m_actionExport->setText(i18n("Export to &PicasaWeb..."));
    m_actionExport->setIcon(KIcon("picasa"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_P));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionExport);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

Plugin_PicasawebExport::~Plugin_PicasawebExport()
{
}

void Plugin_PicasawebExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-picasawebexportplugin-" + QString::number(getpid()) + "/");

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KIPIPicasawebExportPlugin::PicasawebWindow(interface, Tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->show();
}

KIPI::Category Plugin_PicasawebExport::category( KAction* action ) const
{
    if (action == m_actionExport)
        return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return KIPI::ExportPlugin;
}
