/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_debianscreenshots.moc"

// C++ includes

#include <unistd.h>

// KDE includes

#include <kdebug.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "dswindow.h"

namespace KIPIDebianScreenshotsPlugin
{

K_PLUGIN_FACTORY( DebianScreenshotsFactory, registerPlugin<Plugin_DebianScreenshots>(); )
K_EXPORT_PLUGIN ( DebianScreenshotsFactory("kipiplugin_debianscreenshots") )

Plugin_DebianScreenshots::Plugin_DebianScreenshots(QObject* const parent, const QVariantList&)
    : Plugin(DebianScreenshotsFactory::componentData(),
                   parent, "Debian Screenshots Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_DebianScreenshots plugin loaded";

    setUiBaseName("kipiplugin_debianscreenshotsui.rc");
    setupXML();
}

Plugin_DebianScreenshots::~Plugin_DebianScreenshots()
{
}

void Plugin_DebianScreenshots::setup(QWidget* const widget)
{
    m_dlgExport = 0;

    Plugin::setup(widget);
    KIconLoader::global()->addAppDir("kipiplugin_debianscreenshots");
    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_DebianScreenshots::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to &Debian Screenshots..."));
    m_actionExport->setIcon(KIcon("kipi-debianscreenshots"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_D));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction("debianscreenshotsexport", m_actionExport);
}

void Plugin_DebianScreenshots::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-ds-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new DsWindow(tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIDebianScreenshotsPlugin
