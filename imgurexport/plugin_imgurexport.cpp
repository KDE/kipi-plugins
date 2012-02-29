/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
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

#include "plugin_imgurexport.moc"

// C++ includes

#include <unistd.h>

// KDE includes

#include <kdebug.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

using namespace KIPIPlugins;

K_PLUGIN_FACTORY( ImgurExportFactory, registerPlugin<Plugin_ImgurExport>(); )
K_EXPORT_PLUGIN ( ImgurExportFactory("kipiplugin_imgurexport") )

Plugin_ImgurExport::Plugin_ImgurExport(QObject* const parent, const QVariantList& args)
    : Plugin(ImgurExportFactory::componentData(), parent, "ImgurExport")
{
    kDebug(AREA_CODE_LOADING) << "ImgurExport plugin loaded";
    kDebug(AREA_CODE_LOADING) << args;
}

Plugin_ImgurExport::~Plugin_ImgurExport()
{
}

void Plugin_ImgurExport::setup(QWidget* widget)
{
    m_winExport = 0;

    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_imgurexport");

    m_actionExport = actionCollection()->addAction("ImgurExport");
    m_actionExport->setText(i18n("Export to &Imgur..."));
    m_actionExport->setIcon(KIcon("imgur"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_I));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionExport);

    Interface* interface = dynamic_cast<Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_ImgurExport::slotActivate()
{
    Interface* interface = dynamic_cast<Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    if (!m_winExport)
    {
        // We clean it up in the close button
        m_winExport = new ImgurWindow(interface, kapp->activeWindow());
    }
    else
    {
        if (m_winExport->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_winExport->winId());
        }

        KWindowSystem::activateWindow(m_winExport->winId());
    }
    m_winExport->reactivate();

    kDebug() << "We have activated the imgur exporter!";
}

Category Plugin_ImgurExport::category( KAction* action ) const
{
    if (action == m_actionExport)
    {
        return ExportPlugin;
    }

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin;
}
