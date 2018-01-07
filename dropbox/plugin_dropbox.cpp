/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
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

#include "plugin_dropbox.h"

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
#include "dbwindow.h"
#include "kipiplugins_debug.h"

namespace KIPIDropboxPlugin
{

K_PLUGIN_FACTORY(DropboxFactory, registerPlugin<Plugin_Dropbox>(); )

Plugin_Dropbox::Plugin_Dropbox(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Dropbox")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Dropbox Plugin Loaded";

    setUiBaseName("kipiplugin_dropboxui.rc");
    setupXML();

    m_actionExport = 0;
    m_dlgExport    = 0;
}

Plugin_Dropbox::~Plugin_Dropbox()
{
    delete m_dlgExport;

    removeTemporaryDir("dropbox");
}

void Plugin_Dropbox::setup(QWidget* const widget)
{
    m_dlgExport = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "kipi interface is null";
        return;
    }

    setupActions();
}

void Plugin_Dropbox::setupActions()
{
    setDefaultCategory(ExportPlugin);
    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to &Dropbox..."));
    m_actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-dropbox")));
    actionCollection()->setDefaultShortcut(m_actionExport, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_D);

    connect(m_actionExport,SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QString::fromLatin1("dropboxexport"), m_actionExport);
}

void Plugin_Dropbox::slotExport()
{
    QString tmp = makeTemporaryDir("dropbox").absolutePath() + QLatin1Char('/');

    if (!m_dlgExport)
    {
        m_dlgExport = new DBWindow(tmp, QApplication::activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());
        }

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIDropboxPlugin

#include "plugin_dropbox.moc"
