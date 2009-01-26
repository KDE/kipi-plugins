/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
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

#include "plugin_fb.h"
#include "plugin_fb.moc"

// C ANSI includes.
extern "C"
{
#include <unistd.h>
}

// KDE includes.
#include <KDebug>
#include <KConfig>
#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KGenericFactory>
#include <KLibLoader>
#include <KStandardDirs>

// LibKIPI includes.
#include <libkipi/interface.h>

// Local includes.
#include "fbwindow.h"

K_PLUGIN_FACTORY( FbFactory, registerPlugin<Plugin_Fb>(); )
K_EXPORT_PLUGIN ( FbFactory("kipiplugin_fb") )

Plugin_Fb::Plugin_Fb(QObject *parent, const QVariantList &/*args*/)
                   : KIPI::Plugin(FbFactory::componentData(),
                                  parent, "Fb")
{
    kDebug(51001) << "Plugin_Fb plugin loaded";
}

void Plugin_Fb::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_actionExport = actionCollection()->addAction("fbexport");
    m_actionExport->setText(i18n("Export to Facebook..."));
    m_actionExport->setIcon(KIcon("applications-internet"));

    connect(m_actionExport, SIGNAL( triggered(bool) ),
            this, SLOT( slotExport()) );

    addAction(m_actionExport);

    m_actionImport = actionCollection()->addAction("fbimport");
    m_actionImport->setText(i18n("Import from Facebook..."));
    m_actionImport->setIcon(KIcon("applications-internet"));

    connect(m_actionImport, SIGNAL( triggered(bool) ),
            this, SLOT( slotImport()) );

    addAction(m_actionImport);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!";
        m_actionExport->setEnabled(false);
        m_actionImport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
    m_actionImport->setEnabled(true);
}

Plugin_Fb::~Plugin_Fb()
{
}

void Plugin_Fb::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-fb-"
                                           + QString::number(getpid()) + '/');

    // We clean it up in the close button
    m_dlg = new KIPIFbPlugin::FbWindow(interface, tmp, false,
                                       kapp->activeWindow());
    m_dlg->show();
}

void Plugin_Fb::slotImport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-fb-"
                                           + QString::number(getpid()) + '/');

    // We clean it up in the close button
    m_dlg = new KIPIFbPlugin::FbWindow(interface, tmp, true,
                                       kapp->activeWindow());
    m_dlg->show();
}

KIPI::Category Plugin_Fb::category( KAction* action ) const
{
    if (action == m_actionExport)
        return KIPI::ExportPlugin;

    if (action == m_actionImport)
        return KIPI::ImportPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
