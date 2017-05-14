/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageInfo to manage easily
 *               item properties with KIPI host application.
 *
 * Copyright (C) 2017 by Artem Serebriyskiy <v.for.vandal@gmail.com>
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

#include "kpquickinterface.h"

// Qt includes

#include <QMap>
#include <QVariant>

// Libkipi includes

#include <libkipi_version.h>
#include <KIPI/Interface>
#include <KIPI/ImageInfo>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

using namespace KIPI;

namespace KIPIPlugins
{

KPQuickInterface::KPQuickInterface(KIPI::Interface* interface, QObject* parent) :
    QObject(parent),
    m_interface(interface)
{
    if (m_interface) {
        m_currentAlbum = new KPQuickImageCollection(m_interface->currentAlbum(), this);
        m_currentSelection = new KPQuickImageCollection(m_interface->currentSelection(), this);
    }
}

void KPQuickInterface::onCurrentAlbumChanged(bool)
{
    if (m_interface == 0 ) {
        qCritical() << "Signal from non-existent interface pointer";
        return;
    }

    if( m_currentAlbum != 0 ) {
        m_currentAlbum->deleteLater();
    }

    m_currentAlbum = new KPQuickImageCollection(m_interface->currentAlbum(), this);
    emit currentAlbumChanged(m_currentAlbum);
}

void KPQuickInterface::onCurrentSelectionChanged(bool)
{
    if (m_interface == 0 ) {
        qCritical() << "Signal from non-existent interface pointer";
        return;
    }

    if( m_currentSelection != 0 ) {
        m_currentSelection->deleteLater();
    }

    m_currentSelection = new KPQuickImageCollection(m_interface->currentSelection(), this);
    emit currentSelectionChanged(m_currentSelection);
}

}
