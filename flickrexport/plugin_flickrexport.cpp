/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

#include "plugin_flickrexport.moc"

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

#include "flickrwindow.h"
#include "selectuserdlg.h"

namespace KIPIFlickrExportPlugin
{

K_PLUGIN_FACTORY(FlickrExportFactory, registerPlugin<Plugin_FlickrExport>();)
K_EXPORT_PLUGIN(FlickrExportFactory("kipiplugin_flickrexport"))

Plugin_FlickrExport::Plugin_FlickrExport(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(FlickrExportFactory::componentData(), parent, "FlickrExport")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_FlickrExport plugin loaded";

    KIconLoader::global()->addAppDir("kipiplugin_flickrexport");

    setUiBaseName("kipiplugin_flickrexportui.rc");
    setupXML();
}

Plugin_FlickrExport::~Plugin_FlickrExport()
{
}

void Plugin_FlickrExport::setup(QWidget* const widget)
{
    m_dlgFlickr = 0;
    m_dlg23     = 0;
    m_dlgZooomr = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_FlickrExport::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionFlickr = new KAction(this);
    m_actionFlickr->setText(i18n("Export to Flick&r..."));
    m_actionFlickr->setIcon(KIcon("kipi-flickr"));
    m_actionFlickr->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_R));
    
    selectFlickr = new SelectUserDlg(0,"Flickr");
    
    connect(m_actionFlickr, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateFlickr()));

    addAction("flickrexport", m_actionFlickr);

    m_action23 = new KAction(this);
    m_action23->setText(i18n("Export to &23..."));
    m_action23->setIcon(KIcon("kipi-hq"));
    m_action23->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_2));

    select23 = new SelectUserDlg(0,"23");
    
    connect(m_action23, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate23()));

    addAction("23export", m_action23);

    m_actionZooomr = new KAction(this);
    m_actionZooomr->setText(i18n("Export to &Zooomr..."));
    m_actionZooomr->setIcon(KIcon("kipi-zooomr"));
    m_actionZooomr->setShortcut(KShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_Z));

    selectZoomr = new SelectUserDlg(0,"Zooomr");
    
    connect(m_actionZooomr, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateZooomr()));

    addAction("Zooomrexport", m_actionZooomr);
}

void Plugin_FlickrExport::slotActivateFlickr()
{
    selectFlickr->reactivate();
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-flickrexportplugin-") + QString::number(getpid()) + QString("/"));
    
    if (!m_dlgFlickr)
    {
        // We clean it up in the close button
        m_dlgFlickr = new FlickrWindow(tmp, kapp->activeWindow(), "Flickr", selectFlickr);
    }
    else
    {
        if (m_dlgFlickr->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgFlickr->winId());
        }

        KWindowSystem::activateWindow(m_dlgFlickr->winId());
    }

    m_dlgFlickr->reactivate();
}

void Plugin_FlickrExport::slotActivate23()
{
    select23->reactivate();
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-23exportplugin-") + QString::number(getpid()) + QString("/"));
    
    if (!m_dlg23)
    {
        // We clean it up in the close button
        m_dlg23 = new FlickrWindow(tmp, kapp->activeWindow(), "23", select23);
    }
    else
    {
        if (m_dlg23->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlg23->winId());
        }

        KWindowSystem::activateWindow(m_dlg23->winId());
    }

    m_dlg23->reactivate();
}

void Plugin_FlickrExport::slotActivateZooomr()
{
    selectZoomr->reactivate();
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-Zooomrexportplugin-") + QString::number(getpid()) + QString("/"));
    
    if (!m_dlgZooomr)
    {
        // We clean it up in the close button
        m_dlgZooomr = new FlickrWindow(tmp, kapp->activeWindow(), "Zooomr",selectZoomr);
    }
    else
    {
        if (m_dlgZooomr->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_dlgZooomr->winId());
        }

        KWindowSystem::activateWindow(m_dlgZooomr->winId());
    }

    m_dlgZooomr->reactivate();
}

} //namespace KIPIFlickrExportPlugin
