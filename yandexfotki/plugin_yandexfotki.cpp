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

#include "plugin_yandexfotki.moc"

// KDE includes

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kactioncollection.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "yfwindow.h"

namespace KIPIYandexFotkiPlugin
{

K_PLUGIN_FACTORY( Factory, registerPlugin<Plugin_YandexFotki>(); )
K_EXPORT_PLUGIN ( Factory("kipiplugin_yandexfotki") )

Plugin_YandexFotki::Plugin_YandexFotki(QObject* const parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "YandexFotki")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_YandexFotki plugin loaded";

    m_actionExport = 0;
    m_dlgExport    = 0;

    setUiBaseName("kipiplugin_yandexfotkiui.rc");
    setupXML();
}

Plugin_YandexFotki::~Plugin_YandexFotki()
{
}

void Plugin_YandexFotki::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_yandexfotki");

    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_YandexFotki::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to &Yandex.Fotki..."));
    // TODO: icon file
    //m_actionExport->setIcon(KIcon("yandexfotki"));
    m_actionExport->setIcon(KIcon("document-export"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_Y));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction("Yandex.Fotki", m_actionExport);
}

void Plugin_YandexFotki::slotExport()
{
    if (!m_dlgExport)
    {
        // This object will live forever, we will reuse it on future accesses
        // to the plugin.
        m_dlgExport = new YandexFotkiWindow(false, kapp->activeWindow());
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
