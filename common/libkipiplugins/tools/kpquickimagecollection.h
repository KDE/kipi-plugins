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

#ifndef KPQUICKIMAGECOLLECTION_H
#define KPQUICKIMAGECOLLECTION_H

// Qt includes

#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <QObject>

// KIPI includes
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

/** Wrapper for KIPI/ImageCollection. Provides signals and slots 
 * for using class in QML applications
 */

class KIPIPLUGINS_EXPORT KPQuickImageCollection : public QObject 
{
    Q_OBJECT

public:
    KPQuickImageCollection( const KIPI::ImageCollection& collection, QObject* parent = 0 );
    virtual ~KPQuickImageCollection() {}

    QList<QUrl> images() const { return m_collection.images(); }

    Q_PROPERTY( QList<QUrl> images READ images CONSTANT );

private:
    KIPI::ImageCollection m_collection;
};

}

#endif
