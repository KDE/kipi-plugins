/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include <KLocale>
#include <KAction>
#include <KGenericFactory>
#include <KLibLoader>
#include <KConfig>
#include <KDebug>
#include <KApplication>
#include <KActionCollection>
#include <KWindowSystem>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "yfwindow.h"

K_PLUGIN_FACTORY( Factory, registerPlugin<Plugin_YandexFotki>(); )
K_EXPORT_PLUGIN ( Factory("kipiplugin_yandexfotki") )

Plugin_YandexFotki::Plugin_YandexFotki(QObject* parent, const QVariantList&)
    : Plugin(Factory::componentData(), parent, "YandexFotki")
{

    m_dlgExport = 0;
    // m_dlgImport = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_YandexFotki plugin loaded" ;
}

void Plugin_YandexFotki::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_yandexfotki");


    m_actionExport = actionCollection()->addAction("Yandex.Fotki");
    m_actionExport->setText(i18n("Export to &Yandex.Fotki..."));
    // TODO: icon file
    //m_actionExport->setIcon(KIcon("yandexfotki"));
    m_actionExport->setIcon(KIcon("document-export"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_Y));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(m_actionExport);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!";
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

Plugin_YandexFotki::~Plugin_YandexFotki()
{
}

/*
void Plugin_YandexFotki::slotImport()
{

}
*/

void Plugin_YandexFotki::slotExport()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    if (!m_dlgExport)
    {
        m_dlgExport = new KIPIYandexFotkiPlugin::YandexFotkiWindow(interface,
                false, kapp->activeWindow());

        // clean it up in finish signal
        connect(m_dlgExport, SIGNAL( finished(int) ),
            this, SLOT( slotDialogFinished(int) ));
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_YandexFotki::slotDialogFinished(int)
{
    // don't use Qt::WA_DeleteOnClose, delete explicitly
    kDebug() << "Dialog destroyed";
    delete m_dlgExport;
    m_dlgExport = 0;
}

KIPI::Category Plugin_YandexFotki::category( KAction* action ) const
{
    if (action == m_actionExport)
    {
        return KIPI::ExportPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification" ;
    return KIPI::ExportPlugin;
}
