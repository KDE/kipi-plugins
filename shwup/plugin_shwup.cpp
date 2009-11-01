/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

#include "plugin_shwup.h"
#include "plugin_shwup.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <KDebug>
#include <KConfig>
#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KGenericFactory>
#include <KLibLoader>
#include <KStandardDirs>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "swwindow.h"

K_PLUGIN_FACTORY( ShwupFactory, registerPlugin<Plugin_Shwup>(); )
K_EXPORT_PLUGIN ( ShwupFactory("kipiplugin_shwup") )

Plugin_Shwup::Plugin_Shwup(QObject *parent, const QVariantList& /*args*/)
            : KIPI::Plugin(ShwupFactory::componentData(), parent, "Shwup Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Shwup plugin loaded";
}

void Plugin_Shwup::setup(QWidget* widget)
{
    m_dlgExport = 0;

    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_shwup");

    m_actionExport = actionCollection()->addAction("shwupexport");
    m_actionExport->setText(i18n("Export to Shwup..."));
    m_actionExport->setIcon(KIcon("shwup"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_W));

    connect(m_actionExport, SIGNAL( triggered(bool) ),
            this, SLOT( slotExport()) );

    addAction(m_actionExport);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

Plugin_Shwup::~Plugin_Shwup()
{
}

void Plugin_Shwup::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-shwup-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KIPIShwupPlugin::SwWindow(interface, tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

KIPI::Category Plugin_Shwup::category( KAction* action ) const
{
    if (action == m_actionExport)
        return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
