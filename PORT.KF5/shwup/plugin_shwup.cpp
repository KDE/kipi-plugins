/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothee Groleau <kde at timotheegroleau dot com>
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

#include "plugin_shwup.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "swwindow.h"

namespace KIPIShwupPlugin
{

K_PLUGIN_FACTORY( ShwupFactory, registerPlugin<Plugin_Shwup>(); )
K_EXPORT_PLUGIN ( ShwupFactory("kipiplugin_shwup") )

Plugin_Shwup::Plugin_Shwup(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(ShwupFactory::componentData(), parent, "Shwup Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Shwup plugin loaded";

    m_dlgExport    = 0;
    m_actionExport = 0;

    setUiBaseName("kipiplugin_shwupui.rc");
    setupXML();
}

Plugin_Shwup::~Plugin_Shwup()
{
}

void Plugin_Shwup::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_shwup");

    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionExport->setEnabled(true);
}

void Plugin_Shwup::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionExport = new KAction(this);
    m_actionExport->setText(i18n("Export to Shwup..."));
    m_actionExport->setIcon(KIcon("kipi-shwup"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_W));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction("shwupexport", m_actionExport);
}

void Plugin_Shwup::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", "kipi-shwup-" + QString::number(getpid()) + '/');

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new SwWindow(tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

} // namespace KIPIShwupPlugin
