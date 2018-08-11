/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013      by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2008-2016 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#include "plugin_googleservices.h"

//Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include "kputil.h"
#include "gswindow.h"
#include "kipiplugins_debug.h"

namespace KIPIGoogleServicesPlugin
{

K_PLUGIN_FACTORY(GoogleDriveFactory, registerPlugin<Plugin_GoogleServices>(); )

Plugin_GoogleServices::Plugin_GoogleServices(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Google Services")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_GoogleServices Plugin Loaded";

    setUiBaseName("kipiplugin_googleservicesui.rc");
    setupXML();

    m_actionGDriveExport = 0;
    m_actionGPhotoExport = 0;
    m_actionGPhotoImport = 0;

    m_dlgGDriveExport    = 0;
    m_dlgGPhotoExport    = 0;
    m_dlgGPhotoImport    = 0;
}

Plugin_GoogleServices::~Plugin_GoogleServices()
{
    delete m_dlgGDriveExport;
    delete m_dlgGPhotoExport;
    delete m_dlgGPhotoImport;

    removeTemporaryDir("gs");
}

void Plugin_GoogleServices::setup(QWidget* const widget)
{
    m_dlgGDriveExport = 0;
    m_dlgGPhotoExport = 0;
    m_dlgGPhotoImport = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "kipi interface is null";
        return;
    }

    setupActions();
}

void Plugin_GoogleServices::setupActions()
{
    setDefaultCategory(ExportPlugin);
    m_actionGDriveExport = new QAction(this);
    m_actionGDriveExport->setText(i18n("Export to &Google Drive..."));
    m_actionGDriveExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-googledrive")));
    actionCollection()->setDefaultShortcut(m_actionGDriveExport, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_G);

    connect(m_actionGDriveExport, SIGNAL(triggered(bool)),
            this,SLOT(slotGDriveExport()));

    addAction(QString::fromLatin1("googledriveexport"), m_actionGDriveExport);

    m_actionGPhotoExport = new QAction(this);
    m_actionGPhotoExport->setText(i18n("Export to &Google Photos/PicasaWeb..."));
    m_actionGPhotoExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-googlephoto")));
    actionCollection()->setDefaultShortcut(m_actionGPhotoExport, Qt::ALT + Qt::SHIFT + Qt::Key_P);

    connect(m_actionGPhotoExport, SIGNAL(triggered(bool)),
            this, SLOT(slotGPhotoExport()));

    addAction(QString::fromLatin1("googlephotoexport"), m_actionGPhotoExport);

    m_actionGPhotoImport = new QAction(this);
    m_actionGPhotoImport->setText(i18n("Import from &Google Photos/PicasaWeb..."));
    m_actionGPhotoImport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-googlephoto")));
    actionCollection()->setDefaultShortcut(m_actionGPhotoImport, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_P);

    connect(m_actionGPhotoImport, SIGNAL(triggered(bool)),
            this, SLOT(slotGPhotoImport()) );

    addAction(QString::fromLatin1("googlephotoimport"), m_actionGPhotoImport, ImportPlugin);
}

void Plugin_GoogleServices::slotGDriveExport()
{
    QString tmp = makeTemporaryDir("gs").absolutePath() + QLatin1Char('/');

    if (!m_dlgGDriveExport)
    {
        m_dlgGDriveExport = new GSWindow(tmp, QApplication::activeWindow(),
                                         QString::fromLatin1("googledriveexport"));
    }
    else
    {
        if (m_dlgGDriveExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgGDriveExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgGDriveExport->winId());
    }

    m_dlgGDriveExport->reactivate();
}

void Plugin_GoogleServices::slotGPhotoExport()
{
    QString tmp = makeTemporaryDir("gs").absolutePath() + QLatin1Char('/');

    if (!m_dlgGPhotoExport)
    {
        m_dlgGPhotoExport = new GSWindow(tmp, QApplication::activeWindow(),
                                         QString::fromLatin1("googlephotoexport"));
    }
    else
    {
        if (m_dlgGPhotoExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgGPhotoExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgGPhotoExport->winId());
    }

    m_dlgGPhotoExport->reactivate();
}

void Plugin_GoogleServices::slotGPhotoImport()
{
    QString tmp = makeTemporaryDir("gs").absolutePath() + QLatin1Char('/');

    if (!m_dlgGPhotoImport)
    {
        m_dlgGPhotoImport = new GSWindow(tmp, QApplication::activeWindow(),
                                         QString::fromLatin1("googlephotoimport"));
    }
    else
    {
        if (m_dlgGPhotoImport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgGPhotoImport->winId());
        }

        KWindowSystem::activateWindow(m_dlgGPhotoImport->winId());
    }

    m_dlgGPhotoImport->reactivate();
}

} // namespace KIPIGoogleServicesPlugin

#include "plugin_googleservices.moc"
