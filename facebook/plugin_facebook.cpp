/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_facebook.h"

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
#include <KIPI/ImageCollection>

// Local includes

#include "kputil.h"
#include "fbwindow.h"
#include "kipiplugins_debug.h"

namespace KIPIFacebookPlugin
{

K_PLUGIN_FACTORY( FacebookFactory, registerPlugin<Plugin_Facebook>(); )

Plugin_Facebook::Plugin_Facebook(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Facebook")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Facebook plugin loaded";

    setUiBaseName("kipiplugin_facebookui.rc");
    setupXML();

    m_actionExport = 0;
    m_dlgExport    = 0;
}

Plugin_Facebook::~Plugin_Facebook()
{
    delete m_dlgExport;

    removeTemporaryDir("fb");
}

void Plugin_Facebook::setup(QWidget* const widget)
{
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
    m_actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-facebook")));
    actionCollection()->setDefaultShortcut(m_actionExport, Qt::ALT + Qt::SHIFT + Qt::Key_F);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction(QString::fromLatin1("facebookexport"), m_actionExport);
}

void Plugin_Facebook::slotExport()
{
    QString tmp = makeTemporaryDir("fb").absolutePath() + QLatin1Char('/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new FbWindow(tmp, QApplication::activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIFacebookPlugin

#include "plugin_facebook.moc"
