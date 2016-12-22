/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_flickr.h"

//Qt includes

#include <QApplication>

// KDE includes

#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include "flickrwindow.h"
#include "selectuserdlg.h"
#include "kipiplugins_debug.h"

namespace KIPIFlickrPlugin
{

K_PLUGIN_FACTORY(FlickrExportFactory, registerPlugin<Plugin_Flickr>();)

Plugin_Flickr::Plugin_Flickr(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "Flickr")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_Flickr plugin loaded";

    setUiBaseName("kipiplugin_flickrui.rc");
    setupXML();

    m_actionFlickr = 0;
    m_action23     = 0;
    m_actionZooomr = 0;

    m_dlgFlickr    = 0;
    m_dlg23        = 0;
    m_dlgZooomr    = 0;

    selectFlickr   = 0;
    select23       = 0;
    selectZoomr    = 0;
}

Plugin_Flickr::~Plugin_Flickr()
{
    delete m_dlgFlickr;
    delete m_dlg23;
    delete m_dlgZooomr;

    delete selectFlickr;
    delete select23;
    delete selectZoomr;
}

void Plugin_Flickr::setup(QWidget* const widget)
{
    m_dlgFlickr = 0;
    m_dlg23     = 0;
    m_dlgZooomr = 0;

    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    setupActions();
}

void Plugin_Flickr::setupActions()
{
    setDefaultCategory(ExportPlugin);

    m_actionFlickr = new QAction(this);
    m_actionFlickr->setText(i18n("Export to Flick&r..."));
    m_actionFlickr->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-flickr")));
    actionCollection()->setDefaultShortcut(m_actionFlickr, Qt::ALT + Qt::SHIFT + Qt::Key_R);

    selectFlickr = new SelectUserDlg(0,QString::fromLatin1("Flickr"));

    connect(m_actionFlickr, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateFlickr()));

    addAction(QString::fromLatin1("flickrexport"), m_actionFlickr);

    m_action23 = new QAction(this);
    m_action23->setText(i18n("Export to &23..."));
    m_action23->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-hq")));
    actionCollection()->setDefaultShortcut(m_action23, Qt::ALT + Qt::SHIFT + Qt::Key_2);

    select23 = new SelectUserDlg(0,QString::fromLatin1("23"));

    connect(m_action23, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate23()));

    addAction(QString::fromLatin1("23export"), m_action23);

    m_actionZooomr = new QAction(this);
    m_actionZooomr->setText(i18n("Export to &Zooomr..."));
    m_actionZooomr->setIcon(QIcon::fromTheme(QString::fromLatin1("kipi-zooomr")));
    actionCollection()->setDefaultShortcut(m_actionZooomr, Qt::ALT + Qt::SHIFT + Qt::Key_Z);

    selectZoomr = new SelectUserDlg(0,QString::fromLatin1("Zooomr"));

    connect(m_actionZooomr, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateZooomr()));

    addAction(QString::fromLatin1("Zooomrexport"), m_actionZooomr);
}

void Plugin_Flickr::slotActivateFlickr()
{
    selectFlickr->reactivate();

    if (!m_dlgFlickr)
    {
        // We clean it up in the close button
        m_dlgFlickr = new FlickrWindow(QApplication::activeWindow(), QString::fromLatin1("Flickr"), selectFlickr);
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

void Plugin_Flickr::slotActivate23()
{
    select23->reactivate();

    if (!m_dlg23)
    {
        // We clean it up in the close button
        m_dlg23 = new FlickrWindow(QApplication::activeWindow(), QString::fromLatin1("23"), select23);
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

void Plugin_Flickr::slotActivateZooomr()
{
    selectZoomr->reactivate();

    if (!m_dlgZooomr)
    {
        // We clean it up in the close button
        m_dlgZooomr = new FlickrWindow(QApplication::activeWindow(), QString::fromLatin1("Zooomr"), selectZoomr);
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

} // namespace KIPIFlickrPlugin

#include "plugin_flickr.moc"
