/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from 
 *               SmugMug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_smug.moc"

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

#include "smugwindow.h"

namespace KIPISmugPlugin
{

K_PLUGIN_FACTORY( SmugFactory, registerPlugin<Plugin_Smug>(); )
K_EXPORT_PLUGIN ( SmugFactory("kipiplugin_smug") )

Plugin_Smug::Plugin_Smug(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(SmugFactory::componentData(), parent, "Smug")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Smug plugin loaded";

    m_dlgImport    = 0;
    m_dlgExport    = 0;
    m_actionExport = 0;
    m_actionImport = 0;

    setUiBaseName("kipiplugin_smugui.rc");
    setupXML();
}

Plugin_Smug::~Plugin_Smug()
{
}

void Plugin_Smug::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_smug");

    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionImport->setEnabled(true);
    m_actionExport->setEnabled(true);
}

void Plugin_Smug::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to &SmugMug..."));
    m_actionExport->setIcon(KIcon("kipi-smugmug"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_S));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction("smugexport", m_actionExport);

    m_actionImport = new KAction(this);
    m_actionImport->setText(i18n("Import from &SmugMug..."));
    m_actionImport->setIcon(KIcon("kipi-smugmug"));
    m_actionImport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_S));
    m_actionImport->setEnabled(false);

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotImport()) );

    addAction("smugimport", m_actionImport, ImportPlugin);
}

void Plugin_Smug::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-smug-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new SmugWindow(tmp, false, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_Smug::slotImport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-smug-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new SmugWindow(tmp, true, kapp->activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

} // namespace KIPISmugPlugin
