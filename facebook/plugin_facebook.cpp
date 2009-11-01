/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_facebook.h"
#include "plugin_facebook.moc"

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

#include "fbwindow.h"

K_PLUGIN_FACTORY( FacebookFactory, registerPlugin<Plugin_Facebook>(); )
K_EXPORT_PLUGIN ( FacebookFactory("kipiplugin_facebook") )

Plugin_Facebook::Plugin_Facebook(QObject* parent, const QVariantList& /*args*/)
               : KIPI::Plugin(FacebookFactory::componentData(),
                              parent, "Facebook Import/Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Facebook plugin loaded";
}

void Plugin_Facebook::setup(QWidget* widget)
{
    m_dlgImport = 0;
    m_dlgExport = 0;

    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_facebook");

    m_actionExport = actionCollection()->addAction("facebookexport");
    m_actionExport->setText(i18n("Export to &Facebook..."));
    m_actionExport->setIcon(KIcon("facebook"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_F));

    connect(m_actionExport, SIGNAL( triggered(bool) ),
            this, SLOT( slotExport()) );

    addAction(m_actionExport);

    m_actionImport = actionCollection()->addAction("facebookimport");
    m_actionImport->setText(i18n("Import from &Facebook..."));
    m_actionImport->setIcon(KIcon("facebook"));
    m_actionImport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_F));

    connect(m_actionImport, SIGNAL( triggered(bool) ),
            this, SLOT( slotImport()) );

    addAction(m_actionImport);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        m_actionExport->setEnabled(false);
        m_actionImport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
    m_actionImport->setEnabled(true);
}

Plugin_Facebook::~Plugin_Facebook()
{
}

void Plugin_Facebook::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-fb-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KIPIFacebookPlugin::FbWindow(interface, tmp, false, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_Facebook::slotImport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-fb-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new KIPIFacebookPlugin::FbWindow(interface, tmp, true, kapp->activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

KIPI::Category Plugin_Facebook::category( KAction* action ) const
{
    if (action == m_actionExport)
        return KIPI::ExportPlugin;

    if (action == m_actionImport)
        return KIPI::ImportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ExportPlugin;
}
