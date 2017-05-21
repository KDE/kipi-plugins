/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to test bindings to QML
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

#include "qmlimageviewer_plugin.h"

//Qt includes

#include <QApplication>
#include <QAction>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include "kputil.h"
#include "kpquickinit.h"
#include "kipiplugins_debug.h"
#include "qmlimageviewer.h"

using namespace KIPIPlugins;
using namespace KIPI;

K_PLUGIN_FACTORY(QmlImageViewerFactory, registerPlugin<Plugin_QmlImageViewer>(); )

Plugin_QmlImageViewer::Plugin_QmlImageViewer(QObject* const parent, const QVariantList&) :
    Plugin(parent, "QmlImageViewer")
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_GoogleServices Plugin Loaded";

    setUiBaseName("kipiplugin_qmlimageviewerui.rc");
    setupXML();

    InitKIPIQuick(); 
} 

Plugin_QmlImageViewer::~Plugin_QmlImageViewer()
{
}

void Plugin_QmlImageViewer::setup(QWidget* const widget)
{
    Plugin::setup(widget);

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "kipi interface is null";
        return;
    }

    setupActions();
}

void Plugin_QmlImageViewer::setupActions()
{
    setDefaultCategory(ImagesPlugin);
    m_actionView = new QAction(this);
    m_actionView->setText(i18n("View in QmlImageViewer"));

    connect(m_actionView, SIGNAL(triggered(bool)),
            this,SLOT(slotView()));

    addAction(QString::fromLatin1("qmlimageviewer"), m_actionView);
}

void Plugin_QmlImageViewer::slotView()
{
    if( m_viewer == 0 ) {
        m_viewer = new KPQmlImageViewer(0, interface(), QApplication::activeWindow()->windowHandle());
    } else {
        /* TODO: How to implement ?
        if (m_viewer->isMinimized())
        {
            KWindowSystem::unminimizeWindow(m_viewer->winId());
        }*/

        //KWindowSystem::activateWindow(m_viewer->winId());
    }

    //m_viewer->show();
}

#include "qmlimageviewer_plugin.moc"
