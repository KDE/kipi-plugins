/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 * Copyright (C) 2011-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "qmlimageviewer.h"

// Qt includes

#include <QGridLayout>
#include <QProgressBar>
#include <QDebug>
#include <QQmlContext>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QDir>
#include <QWindow>
#include <QQmlApplicationEngine>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes
#include <KIPI/Plugin>
#include <KIPI/PluginLoader>
#include <KIPI/Interface>

// Local includes

#include "kpquickinterface.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;
using namespace KIPI;

KPQmlImageViewer::KPQmlImageViewer(KPAboutData* const about, KIPI::Interface* iface, QWindow* parent)
    : QObject(parent), m_iface(iface)
{
    // setAboutData(about); TODO: KDE specific ?
    Q_UNUSED(about);


    /*
    m_view = new QQuickView();
    m_layout = new QHBoxLayout();

    m_layout->addWidget(m_view);
    setLayout(m_layout);*/

    QString dataDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
            QStringLiteral("kipiplugin_qmlimageviewer"), QStandardPaths::LocateDirectory);

    if( dataDir.isEmpty() ) {
        qCCritical(KIPIPLUGINS_LOG) << "Can't find data dir";
        return;
    } else {
        qCDebug(KIPIPLUGINS_LOG) << "Data dir is " << dataDir;
    }

    QString qmlUi = QDir(dataDir).filePath(QStringLiteral("main.qml"));
    QFileInfo qmlUiInfo(qmlUi);

    if( !qmlUiInfo.exists() ) {
        qCCritical(KIPIPLUGINS_LOG) << "Can't find main ui file";
        return;
    }

    if (m_iface != 0 && !qmlUi.isEmpty()) {
        m_qmlAppEngine = new QQmlApplicationEngine(this);
        QPointer<KPQuickInterface> qmlKPInterface( new KPQuickInterface(m_iface));
        m_qmlAppEngine->rootContext()->setContextProperty(QStringLiteral("KIPIInterface"), qmlKPInterface);

        // Searching for qml file
        m_qmlAppEngine->load(QUrl::fromLocalFile(qmlUi));
    }
}

KPQmlImageViewer::~KPQmlImageViewer()
{
}

