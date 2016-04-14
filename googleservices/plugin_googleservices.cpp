/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

//Qt includes

#include <QAction>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>

// KDE includes

#include <kpluginfactory.h>
#include <kwindowsystem.h>
#include <klocalizedstring.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include "gswindow.h"
#include "kipiplugins_debug.h"

namespace KIPIGoogleServicesPlugin
{

K_PLUGIN_FACTORY(GoogleDriveFactory, registerPlugin<Plugin_GoogleServices>(); )

Plugin_GoogleServices::Plugin_GoogleServices(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Google Services")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_GoogleDrive/PicasaWeb(Google Photos) Plugin Loaded";

    setUiBaseName("kipiplugin_googleservicesui.rc");
    setupXML();

    m_actionGDriveExport = 0;
    m_actionPicasaExport = 0;
    m_actionPicasaImport = 0;

    m_dlgGDriveExport    = 0;
    m_dlgPicasaExport    = 0;
    m_dlgPicasaImport    = 0;
}

Plugin_GoogleServices::~Plugin_GoogleServices()
{
}

void Plugin_GoogleServices::setup(QWidget* const widget)
{
    m_dlgGDriveExport = 0;
    m_dlgPicasaExport = 0;
    m_dlgPicasaImport = 0;

    Plugin::setup(widget);

    if(!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "kipi interface is null";
        return;
    }

    setupActions();
}

void Plugin_GoogleServices::setupActions()
{
    setDefaultCategory(ExportPlugin);// uncomment if import feature is added to google drive
    m_actionGDriveExport = new QAction(this);
    m_actionGDriveExport->setText(i18n("Export to &Google Drive..."));
    m_actionGDriveExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-googledrive")));
    m_actionGDriveExport->setShortcut(QKeySequence(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_G));

    connect(m_actionGDriveExport, SIGNAL(triggered(bool)),
            this,SLOT(slotGDriveExport()));

    addAction(QString::fromLatin1("googledriveexport"), m_actionGDriveExport);

    m_actionPicasaExport = new QAction(this);
    m_actionPicasaExport->setText(i18n("Export to &Google Photos/PicasaWeb..."));
    m_actionPicasaExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-picasa")));
    m_actionPicasaExport->setShortcut(QKeySequence(Qt::ALT+Qt::SHIFT+Qt::Key_P));

    connect(m_actionPicasaExport, SIGNAL(triggered(bool)),
            this, SLOT(slotPicasaExport()));

    addAction(QString::fromLatin1("picasawebexport"), m_actionPicasaExport);

    m_actionPicasaImport = new QAction(this);
    m_actionPicasaImport->setText(i18n("Import from &Google Photos/PicasaWeb..."));
    m_actionPicasaImport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-picasa")));
    m_actionPicasaImport->setShortcut(QKeySequence(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_P));

    connect(m_actionPicasaImport, SIGNAL(triggered(bool)),
            this, SLOT(slotPicasaImport()) );

    addAction(QString::fromLatin1("picasawebimport"), m_actionPicasaImport, ImportPlugin);
}

void Plugin_GoogleServices::slotGDriveExport()
{
    QString tmp = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+ QString::fromLatin1("/")+ QString::fromLatin1("kipi-gs-") + QString::number(getpid()) + QString::fromLatin1("/");
    QDir().mkpath(tmp);

    if(!m_dlgGDriveExport)
    {
        m_dlgGDriveExport = new GSWindow(tmp, QApplication::activeWindow(),
                                         QString::fromLatin1("googledriveexport"));
    }
    else
    {
        if(m_dlgGDriveExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgGDriveExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgGDriveExport->winId());
    }

    m_dlgGDriveExport->reactivate();
}

void Plugin_GoogleServices::slotPicasaExport()
{
    QString tmp = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+ QString::fromLatin1("/")+ QString::fromLatin1("kipi-gs-") + QString::number(getpid()) + QString::fromLatin1("/");
    QDir().mkpath(tmp);

    if(!m_dlgPicasaExport)
    {
        m_dlgPicasaExport = new GSWindow(tmp, QApplication::activeWindow(),
                                         QString::fromLatin1("picasawebexport"));
    }
    else
    {
        if(m_dlgPicasaExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgPicasaExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgPicasaExport->winId());
    }

    m_dlgPicasaExport->reactivate();
}

void Plugin_GoogleServices::slotPicasaImport()
{
    QString tmp = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+ QString::fromLatin1("/")+ QString::fromLatin1("kipi-gs-") + QString::number(getpid()) + QString::fromLatin1("/");
    QDir().mkpath(tmp);

    if(!m_dlgPicasaImport)
    {
        m_dlgPicasaImport = new GSWindow(tmp, QApplication::activeWindow(),
                                         QString::fromLatin1("picasawebimport"));
    }
    else
    {
        if(m_dlgPicasaImport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgPicasaImport->winId());
        }

        KWindowSystem::activateWindow(m_dlgPicasaImport->winId());
    }

    m_dlgPicasaImport->reactivate();
}

} // namespace KIPIGoogleServicesPlugin

#include "plugin_googleservices.moc"
