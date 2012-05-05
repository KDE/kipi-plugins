/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-17-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

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
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "picasawebwindow.h"

namespace KIPIPicasawebExportPlugin
{

K_PLUGIN_FACTORY( PicasawebExportFactory, registerPlugin<Plugin_PicasawebExport>(); )
K_EXPORT_PLUGIN ( PicasawebExportFactory("kipiplugin_picasawebexport") )

Plugin_PicasawebExport::Plugin_PicasawebExport(QObject* const parent, const QVariantList&)
    : Plugin(PicasawebExportFactory::componentData(), parent, "PicasawebExport")
{
    m_dlgExport = 0;
    m_dlgImport = 0;

    kDebug(AREA_CODE_LOADING) << "Plugin_PicasawebExport plugin loaded" ;
}

void Plugin_PicasawebExport::setup(QWidget* widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_picasawebexport");

    m_actionExport = actionCollection()->addAction("picasawebexport");
    m_actionExport->setText(i18n("Export to &PicasaWeb..."));
    m_actionExport->setIcon(KIcon("picasa"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_P));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction(m_actionExport);

    m_actionImport = actionCollection()->addAction("picasawebimport");
    m_actionImport->setText(i18n("Import from &PicasaWeb..."));
    m_actionImport->setIcon(KIcon("picasa"));
    m_actionImport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_P));

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotImport()) );

    addAction(m_actionImport);

    Interface* interface = dynamic_cast<Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        m_actionImport->setEnabled(false);
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionImport->setEnabled(true);
    m_actionExport->setEnabled(true);
}

Plugin_PicasawebExport::~Plugin_PicasawebExport()
{
}

void Plugin_PicasawebExport::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-picasawebexportplugin-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new PicasawebWindow(tmp, false, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_PicasawebExport::slotImport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-picasawebexportplugin-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new PicasawebWindow(tmp, true, kapp->activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

Category Plugin_PicasawebExport::category( KAction* action ) const
{
    if (action == m_actionExport)
        return ExportPlugin;
    else if (action == m_actionImport)
        return ImportPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return ExportPlugin;
}

} // namespace KIPIPicasawebExportPlugin
