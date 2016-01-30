/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#include "plugin_remotestorage.h"

// Qt includes

#include <QAction>
#include <QApplication>

// KDE includes

#include <kactioncollection.h>
#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowsystem.h>
#include <kpluginfactory.h>

// LibKipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "KioExportWindow.h"
#include "KioImportWindow.h"

namespace KIPIRemoteStoragePlugin
{

K_PLUGIN_FACTORY( KioFactory, registerPlugin<Plugin_RemoteStorage>(); )

Plugin_RemoteStorage::Plugin_RemoteStorage(QObject* const parent, const QVariantList&)
    : Plugin(parent, "RemoteStorage")
{
    m_actionExport = 0;
    m_actionImport = 0;
    m_dlgExport    = 0;
    m_dlgImport    = 0;

    qCDebug(KIPIPLUGINS_LOG) << "Plugin_RemoteStorage plugin loaded";

    setUiBaseName("kipiplugin_remotestorageui.rc");
    setupXML();
}

Plugin_RemoteStorage::~Plugin_RemoteStorage()
{
}

void Plugin_RemoteStorage::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    setupActions();

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
    m_actionImport->setEnabled(true);
}

void Plugin_RemoteStorage::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to remote storage..."));
    m_actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("folder-remote")));
    m_actionExport->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_K));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateExport()));

    addAction(QString::fromLatin1("remotestorageexport"), m_actionExport);

    // import
    m_actionImport = new QAction(this);
    m_actionImport->setText(i18n("Import from remote storage..."));
    m_actionImport->setIcon(QIcon::fromTheme(QString::fromLatin1("folder-remote")));
    m_actionImport->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_I));
    m_actionImport->setEnabled(false);

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateImport()));

    addAction(QString::fromLatin1("remotestorageimport"), m_actionImport, ImportPlugin);
}

void Plugin_RemoteStorage::slotActivateExport()
{
    qCDebug(KIPIPLUGINS_LOG) << "Starting Remote Storage export";

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new KioExportWindow(QApplication::activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_RemoteStorage::slotActivateImport()
{
    qCDebug(KIPIPLUGINS_LOG) << "Starting Remote Storage import";

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new KioImportWindow(QApplication::activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

} // namespace KIPIRemoteStoragePlugin

#include "plugin_remotestorage.moc"
