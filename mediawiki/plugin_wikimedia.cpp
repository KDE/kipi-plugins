/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// To disable warnings under MSVC2008 about getpid().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_wikimedia.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kdebug.h>
#include <KConfig>
#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KGenericFactory>
#include <KLibLoader>
#include <KStandardDirs>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "wmwindow.h"

namespace KIPIWikiMediaPlugin
{

K_PLUGIN_FACTORY( WikiMediaFactory, registerPlugin<Plugin_WikiMedia>(); )
K_EXPORT_PLUGIN ( WikiMediaFactory("kipiplugin_wikimedia") )

Plugin_WikiMedia::Plugin_WikiMedia(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(WikiMediaFactory::componentData(),
                   parent, "Wikimedia Commons Export")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_WikiMedia plugin loaded";
}

void Plugin_WikiMedia::setup(QWidget* widget)
{
    m_dlgExport = 0;
    Plugin::setup(widget);

    KIconLoader::global()->addAppDir("kipiplugin_wikimedia");

    m_actionExport = actionCollection()->addAction("wikimediaexport");
    m_actionExport->setText(i18n("Export to &WikiMedia Commons..."));
    m_actionExport->setIcon(KIcon("wikimedia"));

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction(m_actionExport);

    Interface* interface = dynamic_cast<Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        m_actionExport->setEnabled(false);
        return;
    }

    m_actionExport->setEnabled(true);
}

Plugin_WikiMedia::~Plugin_WikiMedia()
{
}

void Plugin_WikiMedia::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-mediawiki-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new WMWindow(tmp, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        m_dlgExport->reactivate();
    }
}

Category Plugin_WikiMedia::category(KAction* action) const
{
    if (action == m_actionExport)
        return ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin;
}

} // namespace KIPIWikiMediaPlugin
