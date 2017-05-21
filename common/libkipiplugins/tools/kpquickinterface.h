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

#ifndef KPQUICKINTERFACE_H
#define KPQUICKINTERFACE_H

// Qt includes

#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <QObject>
#include <QPointer>

// Libkipi includes
//
#include <KIPI/Interface>

// Local includes

#include "kpquickimagecollection.h"
#include "kipiplugins_export.h"

namespace KIPIPlugins
{

/** Wrapper for KIPI/Interface. Provides signals and slots 
 * for using class in QML applications
 */

class KIPIPLUGINS_EXPORT KPQuickInterface : public QObject 
{
    Q_OBJECT

public:
    KPQuickInterface(KIPI::Interface* interface, QObject* parent = 0);

    const KPQuickImageCollection* currentAlbum() const { return m_currentAlbum; }
    KPQuickImageCollection* currentAlbum() { return m_currentAlbum; }

    const KPQuickImageCollection* currentSelection() const { return m_currentSelection; }
    KPQuickImageCollection* currentSelection() { return m_currentSelection; }

    operator QString() const;

Q_SIGNALS:
    void currentAlbumChanged(KPQuickImageCollection*);
    void currentSelectionChanged(KPQuickImageCollection*);

public:
    Q_PROPERTY( KPQuickImageCollection* currentAlbum READ currentAlbum NOTIFY currentAlbumChanged );
    Q_PROPERTY( KPQuickImageCollection* currentSelection READ currentSelection NOTIFY currentSelectionChanged );
            
private Q_SLOTS:
    void onCurrentAlbumChanged(bool);
    void onCurrentSelectionChanged(bool);

private:
    QPointer<KIPI::Interface> m_interface;
    QPointer<KPQuickImageCollection> m_currentAlbum; 
    QPointer<KPQuickImageCollection> m_currentSelection;
};

}

#endif

