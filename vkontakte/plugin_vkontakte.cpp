/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte.ru web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2011, 2015  Alexander Potashev <aspotashev@gmail.com>
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

#include "plugin_vkontakte.h"

// Qt includes

#include <QtWidgets/QApplication>

// KDE includes

#include <KPluginFactory>
#include <KLocalizedString>
#include <KWindowSystem>

// Local includes

#include "vkwindow.h"
#include "kipiplugins_debug.h"

namespace KIPIVkontaktePlugin
{

K_PLUGIN_FACTORY( Factory, registerPlugin<Plugin_Vkontakte>(); )

Plugin_Vkontakte::Plugin_Vkontakte(QObject* const parent, const QVariantList&)
    : Plugin(parent, "VKontakte")
{
    m_dlgExport    = 0;
    m_actionExport = 0;

    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Vkontakte plugin loaded";

    setUiBaseName("kipiplugin_vkontakteui.rc");
    setupXML();
}

Plugin_Vkontakte::~Plugin_Vkontakte()
{
}

void Plugin_Vkontakte::setup(QWidget* const widget)
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

void Plugin_Vkontakte::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new QAction(this);
    m_actionExport->setText(i18n("Export to &VKontakte..."));
    // TODO: icon file
    //m_actionExport->setIcon(KIcon("vkontakte"));
    m_actionExport->setIcon(QIcon::fromTheme(QStringLiteral("preferences-web-browser-shortcuts")));
    //m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_Y));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(QStringLiteral("VKontakte"), m_actionExport);
}

void Plugin_Vkontakte::slotExport()
{
    if (!m_dlgExport)
    {
        // This object will live forever, we will reuse it on future accesses
        // to the plugin.
        m_dlgExport = new VkontakteWindow(false, QApplication::activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->startReactivation();
}

} // namespace KIPIVkontaktePlugin

#include "plugin_vkontakte.moc"
