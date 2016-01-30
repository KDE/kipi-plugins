/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_rajce.h"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QAction>
#include <QApplication>
#include <QKeySequence>

// KDE includes

#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <kwindowsystem.h>
#include <kpluginfactory.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"
#include "kputil.h"
#include "rajcewindow.h"

namespace KIPIRajcePlugin
{

K_PLUGIN_FACTORY( RajceFactory, registerPlugin<Plugin_Rajce>(); )

Plugin_Rajce::Plugin_Rajce(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Rajce"),
      m_actionExport(0),
      m_dlgExport(0)
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Rajce plugin loaded";

    setUiBaseName("kipiplugin_rajceui.rc");
    setupXML();
}

Plugin_Rajce::~Plugin_Rajce()
{
}

void Plugin_Rajce::setup(QWidget* const widget)
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

void Plugin_Rajce::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to &Rajce.net..."));
    m_actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-rajce")));
    m_actionExport->setShortcut(QKeySequence(Qt::ALT+Qt::SHIFT+Qt::Key_J));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QString::fromLatin1("rajceexport"), m_actionExport);
}

void Plugin_Rajce::slotExport()
{
    QString tmp = makeTemporaryDir("kipi-rajce").absolutePath() + QString::fromLatin1("/");

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new RajceWindow(tmp, QApplication::activeWindow());
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

} // namespace KIPIRajcePlugin

#include "plugin_rajce.moc"
