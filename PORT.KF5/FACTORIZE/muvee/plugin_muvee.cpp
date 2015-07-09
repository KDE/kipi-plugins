/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to cloud.muvee.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothee Groleau <kde at timotheegroleau dot com>
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

#include "plugin_muvee.h"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include "kipiplugins_debug.h"
#include <kconfig.h>
#include <QApplication>
#include <QAction>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "swwindow.h"

namespace KIPIMuveePlugin
{

K_PLUGIN_FACTORY( MuveeFactory, registerPlugin<Plugin_Muvee>(); )

Plugin_Muvee::Plugin_Muvee(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Muvee")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Muvee plugin loaded";

    m_dlgExport    = 0;
    m_actionExport = 0;

    setUiBaseName("kipiplugin_muveeui.rc");
    setupXML();
}

Plugin_Muvee::~Plugin_Muvee()
{
}

void Plugin_Muvee::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    setupActions();

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_Muvee::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to Muvee Cloud..."));
    m_actionExport->setIcon(QIcon::fromTheme("kipi-muvee"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_W));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction("muveeexport", m_actionExport);
}

void Plugin_Muvee::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-muvee-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new SwWindow(tmp, QApplication::activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIMuveePlugin

#include "plugin_muvee.moc"
