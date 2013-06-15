/* ============================================================
 *
 * This file is a part of kipi-plugins project
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_rajceexport.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kaction.h>
#include <klibloader.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kshortcut.h>
#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>
#include <kapplication.h>

// Libkipi includes

#include <libkipi/interface.h>

namespace KIPIRajceExportPlugin
{

K_PLUGIN_FACTORY( RajceExportFactory, registerPlugin<Plugin_RajceExport>(); )
K_EXPORT_PLUGIN ( RajceExportFactory("kipiplugin_rajceexport") )

Plugin_RajceExport::Plugin_RajceExport(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(RajceExportFactory::componentData(), parent, "RajceExport"),
      m_actionExport(0),
      m_dlgExport(0)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_RajceExport plugin loaded";

    setUiBaseName("kipiplugin_rajceexportui.rc");
    setupXML();
}

Plugin_RajceExport::~Plugin_RajceExport()
{
}

void Plugin_RajceExport::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_rajceexport");

    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_RajceExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to &Rajce.net..."));
    m_actionExport->setIcon(KIcon("kipi-rajce"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_J));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()));

    addAction("rajceexport", m_actionExport);
}

void Plugin_RajceExport::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-rajceexportplugin-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new RajceWindow(tmp, kapp->activeWindow());
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

} // namespace KIPIRajceExportPlugin
