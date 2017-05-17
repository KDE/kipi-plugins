/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : help wrapper around libkipi ImageCollection to manage easily
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

class KIPIPLUGINS_EXPORT KPQuickImageCollection : public QObject, public KIPI::ImageCollection
{
    Q_OBJECT

public:
    KPQuickImageCollection( const KIPI::ImageCollection& collection, QObject* parent = 0 );
    virtual ~KPQuickImageCollection() {}

    operator QString() const;

    Q_PROPERTY( QList<QUrl> images READ images CONSTANT );
    Q_PROPERTY( QString name READ name CONSTANT );
    Q_PROPERTY( QString comment READ comment CONSTANT);
    Q_PROPERTY( QString category READ category CONSTANT);
    Q_PROPERTY( QDate date READ date CONSTANT);
    Q_PROPERTY( QUrl url READ url CONSTANT);
    Q_PROPERTY( QUrl uploadUrl READ uploadUrl CONSTANT);
    Q_PROPERTY( QUrl uploadRootUrl READ uploadRootUrl CONSTANT);
    Q_PROPERTY( bool isDirectory READ isDirectory CONSTANT);
    Q_PROPERTY( QString uploadRootName READ uploadRootName CONSTANT);
    Q_PROPERTY( bool isValid READ isValid CONSTANT);
};

}

#endif
