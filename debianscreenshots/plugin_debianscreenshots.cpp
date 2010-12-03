/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#include "plugin_debianscreenshots.h"
#include "plugin_debianscreenshots.moc"

// KDE includes
#include <kdebug.h>
#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KGenericFactory>
#include <KLibLoader>
#include <KIconLoader>
#include <KStandardDirs>
#include <kwindowsystem.h>

// LibKIPI includes
#include <libkipi/interface.h>

// Local includes
#include "dswindow.h"
//#include "dsjob.h"

K_PLUGIN_FACTORY( DebianScreenshotsFactory, registerPlugin<Plugin_DebianScreenshots>(); )
K_EXPORT_PLUGIN ( DebianScreenshotsFactory("kipiplugin_debianscreenshots") )

Plugin_DebianScreenshots::Plugin_DebianScreenshots(QObject* parent, const QVariantList& /*args*/)
               : KIPI::Plugin(DebianScreenshotsFactory::componentData(),
                              parent, "Debian Screenshots Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_DebianScreenshots plugin loaded";
}

void Plugin_DebianScreenshots::setup(QWidget* widget)
{
    m_dlgExport = 0;

    kWarning() << Q_FUNC_INFO;

    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_debianscreenshots");

    m_actionExport = actionCollection()->addAction("debianscreenshotsexport");
    m_actionExport->setText(i18n("Export to &Debian Screenshots..."));
    m_actionExport->setIcon(KIcon("debianscreenshots"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_F));

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

Plugin_DebianScreenshots::~Plugin_DebianScreenshots()
{
}

void Plugin_DebianScreenshots::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-ds-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KIPIDebianScreenshotsPlugin::DsWindow(interface, tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

KIPI::Category Plugin_DebianScreenshots::category( KAction* /* action */ ) const
{
    return KIPI::ExportPlugin;
}

//KJob* Plugin_DebianScreenshots::exportFiles(const QString& album)
//{
//    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
//    return new KIPIDebianScreenshotsPlugin::DebianScreenshotsJob(album, interface->currentSelection().images());
//}
