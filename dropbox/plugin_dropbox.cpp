/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to import/export images to Dropbox web service
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_dropbox.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "dbwindow.h"

namespace KIPIDropboxPlugin
{

K_PLUGIN_FACTORY(DropboxFactory, registerPlugin<Plugin_Dropbox>(); )
K_EXPORT_PLUGIN(DropboxFactory("kipiplugin_dropbox"))

Plugin_Dropbox::Plugin_Dropbox(QObject* const parent,const QVariantList& /*args*/)
    : Plugin(DropboxFactory::componentData(),parent,"Dropbox Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Dropbox Plugin Loaded";

    KIconLoader::global()->addAppDir("kipiplugin_dropbox");
    setUiBaseName("kipiplugin_dropboxui.rc");
    setupXML();
}

Plugin_Dropbox::~Plugin_Dropbox()
{
}

void Plugin_Dropbox::setup(QWidget* const widget)
{
    m_dlgExport = 0;

    Plugin::setup(widget);

    if(!interface())
    {
        kDebug() << "kipi interface is null";
        return;
    }

    setupActions();
}

void Plugin_Dropbox::setupActions()
{
    setDefaultCategory(ExportPlugin);// uncomment if import feature is added to google drive
    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to &Dropbox..."));
    m_actionExport->setIcon(KIcon("kipi-dropbox"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_D));

    connect(m_actionExport,SIGNAL(triggered(bool)),
            this,SLOT(slotExport()));

    addAction("dropboxexport",m_actionExport);
}

void Plugin_Dropbox::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp",QString("kipi-db-") + QString::number(getpid()) + QString("/"));

    if(!m_dlgExport)
    {
        m_dlgExport = new DBWindow(tmp,kapp->activeWindow());
    }
    else
    {
        if(m_dlgExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIDropboxPlugin
