/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_facebook.h"

// Qt includes

#include <QApplication>
#include <QAction>
#include <QtCore/QDir>

// KDE includes

#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>
#include <KPluginFactory>
#include <KLocalizedString>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "kputil.h"
#include "fbwindow.h"

namespace KIPIFacebookPlugin
{

K_PLUGIN_FACTORY( FacebookFactory, registerPlugin<Plugin_Facebook>(); )

Plugin_Facebook::Plugin_Facebook(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Facebook")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Facebook plugin loaded";
    
    setUiBaseName("kipiplugin_facebookui.rc");
    setupXML();
    
    m_actionImport = 0;
    m_actionExport = 0;
    m_dlgImport    = 0;
    m_dlgExport    = 0;
}

Plugin_Facebook::~Plugin_Facebook()
{
}

void Plugin_Facebook::setup(QWidget* const widget)
{
    m_dlgImport = 0;
    m_dlgExport = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_Facebook::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to &Facebook..."));
    m_actionExport->setIcon(QIcon::fromTheme("kipi-facebook"));
    m_actionExport->setShortcut(QKeySequence(Qt::ALT+Qt::SHIFT+Qt::Key_F));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction("facebookexport", m_actionExport);

    m_actionImport = new QAction(this);
    m_actionImport->setText(i18n("Import from &Facebook..."));
    m_actionImport->setIcon(QIcon::fromTheme("kipi-facebook"));
    m_actionImport->setShortcut(QKeySequence(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_F));

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotImport()) );

    addAction("facebookimport", m_actionImport, ImportPlugin);
}

void Plugin_Facebook::slotExport()
{
    QString tmp = makeTemporaryDir("kipi-fb").absolutePath() + QString("/");

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new FbWindow(tmp, false, QApplication::activeWindow());
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
    QString tmp = makeTemporaryDir("kipi-fb").absolutePath() + QString("/");

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new FbWindow(tmp, true, QApplication::activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

} // namespace KIPIFacebookPlugin

#include "plugin_facebook.moc"
