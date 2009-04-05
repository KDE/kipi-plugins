/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-17-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_picasawebexport.h"
#include "plugin_picasawebexport.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "picasawebwindow.h"

K_PLUGIN_FACTORY( PicasawebExportFactory, registerPlugin<Plugin_PicasawebExport>(); )
K_EXPORT_PLUGIN ( PicasawebExportFactory("kipiplugin_picasawebexport") )

Plugin_PicasawebExport::Plugin_PicasawebExport(QObject *parent, const QVariantList &)
                      : KIPI::Plugin(PicasawebExportFactory::componentData(), parent, "PicasawebExport")
{
    kDebug(51001) << "Plugin_PicasawebExport plugin loaded" << endl;
}

void Plugin_PicasawebExport::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_picasawebexport");

    m_action = actionCollection()->addAction("picasawebexport");
    m_action->setText(i18n("Export to &Picasaweb..."));
    m_action->setIcon(KIcon("picasa"));
    m_action->setShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_P);

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        m_action->setEnabled(false);
        return;
    }
    m_action->setEnabled(true);
}

Plugin_PicasawebExport::~Plugin_PicasawebExport()
{
}

void Plugin_PicasawebExport::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
    if (!interface)
    {
        kError(51000) << "Kipi interface is null!" << endl;
        return;
    }

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-picasawebexportplugin-" + QString::number(getpid()) + "/");

    m_dlg = new KIPIPicasawebExportPlugin::PicasawebWindow(interface,Tmp,kapp->activeWindow());
    m_dlg->show();
}

KIPI::Category Plugin_PicasawebExport::category( KAction* action ) const
{
    if (action == m_action)
        return KIPI::ExportPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::ExportPlugin;
}
