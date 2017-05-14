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

#ifndef QML_IMAGE_VIEWER_H
#define QML_IMAGE_VIEWER_H

// Qt includes

#include <QUrl>
#include <QQuickView>
#include <QDialog>

// Libkipi
#include <KIPI/Interface>

// Local includes

#include "kpaboutdata.h"

using namespace KIPIPlugins;

class QHBoxLayout; // TODO: Not needed
class QQmlApplicationEngine;

class KPQmlImageViewer : public QObject
{
    Q_OBJECT

public:

    KPQmlImageViewer(KPAboutData* const about, KIPI::Interface* iface, QWindow* parent = 0);
    ~KPQmlImageViewer();

private:
    /* TODO: REMOVE
    QHBoxLayout* m_layout = 0;
    QQuickView* m_view = 0; */
    KIPI::Interface* m_iface = 0;
    QQmlApplicationEngine* m_qmlAppEngine = 0;
};

#endif // QML_IMAGE_VIEWER_H
