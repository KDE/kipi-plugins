/* ============================================================
 *
 * This file is a part of digiKam project
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

#include "plugin_rajce.h"

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
#include "rajcewindow.h"
#include "kipiplugins_debug.h"

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
    delete m_dlgExport;

    removeTemporaryDir("rajce");
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
    actionCollection()->setDefaultShortcut(m_actionExport, Qt::ALT + Qt::SHIFT + Qt::Key_J);
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QString::fromLatin1("rajceexport"), m_actionExport);
}

void Plugin_Rajce::slotExport()
{
    QString tmp = makeTemporaryDir("rajce").absolutePath() + QLatin1Char('/');

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
