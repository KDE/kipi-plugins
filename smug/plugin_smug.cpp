/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from
 *               SmugMug web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_smug.h"

// Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kputil.h"
#include "smugwindow.h"
#include "kipiplugins_debug.h"

namespace KIPISmugPlugin
{

K_PLUGIN_FACTORY( SmugFactory, registerPlugin<Plugin_Smug>(); )

Plugin_Smug::Plugin_Smug(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Smug")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Smug plugin loaded";

    m_dlgImport    = 0;
    m_dlgExport    = 0;
    m_actionExport = 0;
    m_actionImport = 0;

    setUiBaseName("kipiplugin_smugui.rc");
    setupXML();
}

Plugin_Smug::~Plugin_Smug()
{
    delete m_dlgImport;
    delete m_dlgExport;

    removeTemporaryDir("smug");
}

void Plugin_Smug::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    setupActions();

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    m_actionImport->setEnabled(true);
    m_actionExport->setEnabled(true);
}

void Plugin_Smug::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to &SmugMug..."));
    m_actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-smugmug")));
    actionCollection()->setDefaultShortcut(m_actionExport, Qt::ALT + Qt::SHIFT + Qt::Key_S);
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction(QString::fromLatin1("smugexport"), m_actionExport);

    m_actionImport = new QAction(this);
    m_actionImport->setText(i18n("Import from &SmugMug..."));
    m_actionImport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-smugmug")));
    actionCollection()->setDefaultShortcut(m_actionImport, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_S);
    m_actionImport->setEnabled(false);

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotImport()) );

    addAction(QString::fromLatin1("smugimport"), m_actionImport, ImportPlugin);
}

void Plugin_Smug::slotExport()
{
    QString tmp = makeTemporaryDir("smug").absolutePath() + QLatin1Char('/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new SmugWindow(tmp, false, QApplication::activeWindow());
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
    QString tmp = makeTemporaryDir("smug").absolutePath() + QLatin1Char('/');

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new SmugWindow(tmp, true, QApplication::activeWindow());
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

#include "plugin_smug.moc"
