/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "plugin_rajceexport.h"

#include "rajcewindow.h"

#include <kaction.h>
#include <klibloader.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kshortcut.h>
#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>
#include <kapplication.h>

#include <libkipi/interface.h>

K_PLUGIN_FACTORY( RajceExportFactory, registerPlugin<Plugin_RajceExport>(); )
K_EXPORT_PLUGIN ( RajceExportFactory("kipiplugin_rajceexport") )

Plugin_RajceExport::Plugin_RajceExport(QObject* parent, const QVariantList& args):
        Plugin(RajceExportFactory::componentData(), parent, "RajceExport"), m_dlgExport(0)
{

}

Plugin_RajceExport::~Plugin_RajceExport()
{

}

KIPI::Category Plugin_RajceExport::category(KAction* action) const
{
    if (action == m_actionExport)
        return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action";
    return KIPI::ExportPlugin;
}

void Plugin_RajceExport::setup(QWidget* widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_rajceexport");

    m_actionExport = actionCollection()->addAction("rajceexport");
    m_actionExport->setText(i18n("Export to &Rajce.net..."));
    m_actionExport->setIcon(KIcon("rajce"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_J));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(m_actionExport);

    m_actionExport->setEnabled(true);
}

void Plugin_RajceExport::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-rajceexportplugin-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KIPIRajceExportPlugin::RajceWindow(interface, Tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}
