/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte.ru web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
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

#include "vkwindow.h"

K_PLUGIN_FACTORY( Factory, registerPlugin<Plugin_Vkontakte>(); )
K_EXPORT_PLUGIN ( Factory("kipiplugin_vkontakte") )

Plugin_Vkontakte::Plugin_Vkontakte(QObject *parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "VKontakte")
{

    m_dlgExport = 0;
    // m_dlgImport = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_Vkontakte plugin loaded" ;
}

void Plugin_Vkontakte::setup(QWidget *widget)
{
    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_vkontakte");


    m_actionExport = actionCollection()->addAction("VKontakte");
    m_actionExport->setText(i18n("Export to &VKontakte..."));
    // TODO: icon file
    //m_actionExport->setIcon(KIcon("vkontakte"));
    m_actionExport->setIcon(KIcon("preferences-web-browser-shortcuts"));
    //m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_Y));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(m_actionExport);

    KIPI::Interface *interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!";
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

Plugin_Vkontakte::~Plugin_Vkontakte()
{
}

/*
void Plugin_Vkontakte::slotImport()
{

}
*/

void Plugin_Vkontakte::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    if (!m_dlgExport)
    {
        // This object will live forever, we will reuse it on future accesses
        // to the plugin.
        m_dlgExport = new KIPIVkontaktePlugin::VkontakteWindow(interface,
                false, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->startReactivation();
}

KIPI::Category Plugin_Vkontakte::category(KAction *action) const
{
    if (action == m_actionExport)
    {
        return KIPI::ExportPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification" ;
    return KIPI::ExportPlugin;
}

#include "plugin_vkontakte.moc"
