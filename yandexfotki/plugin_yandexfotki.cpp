/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#include "plugin_yandexfotki.h"

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
#include "yfwindow.h"
#include "kipiplugins_debug.h"

namespace KIPIYandexFotkiPlugin
{

K_PLUGIN_FACTORY( Factory, registerPlugin<Plugin_YandexFotki>(); )

Plugin_YandexFotki::Plugin_YandexFotki(QObject* const parent, const QVariantList&)
    : Plugin(parent, "YandexFotki")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_YandexFotki plugin loaded";

    m_dlgExport    = 0;
    m_actionExport = 0;

    setUiBaseName("kipiplugin_yandexfotkiui.rc");
    setupXML();
}

Plugin_YandexFotki::~Plugin_YandexFotki()
{
    delete m_dlgExport;

    removeTemporaryDir("yandexfotki");
}

void Plugin_YandexFotki::setup(QWidget* const widget)
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

void Plugin_YandexFotki::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to &Yandex.Fotki..."));
    // TODO: icon file
    //m_actionExport->setIcon(QIcon::fromTheme("yandexfotki"));
    m_actionExport->setIcon(QIcon::fromTheme(QString::fromLatin1("folder-html")));
    actionCollection()->setDefaultShortcut(m_actionExport, Qt::ALT + Qt::SHIFT + Qt::Key_Y);
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QString::fromLatin1("Yandex.Fotki"), m_actionExport);
}

void Plugin_YandexFotki::slotExport()
{
    if (!m_dlgExport)
    {
        // This object will live forever, we will reuse it on future accesses
        // to the plugin.
        m_dlgExport = new YandexFotkiWindow(false, QApplication::activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIYandexFotkiPlugin

#include "plugin_yandexfotki.moc"
