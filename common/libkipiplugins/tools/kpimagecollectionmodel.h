/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-06
 * Description : QAbstractItmModel interface around libkipi ImageCollection to manage easily
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

#ifndef KPQUICKIMAGECOLLECTIONMODEL_H
#define KPQUICKIMAGECOLLECTIONMODEL_H

// Qt includes

#include <QUrl>
#include <QObject>
#include <QPointer>
#include <QAbstractListModel>

// KIPI includes
#include <KIPI/ImageCollection>

// Local includes
#include "kpquickimagecollection.h"
#include "kipiplugins_export.h"

namespace KIPIPlugins
{

/** Wrapper for KIPI/ImageCollection. Provides signals and slots 
 * for using class in QML applications
 */

class KIPIPLUGINS_EXPORT KPImageCollectionModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ImageRoles {
        UrlRole = Qt::UserRole + 1,
        ThumbnailUrlRole,
        PreviewUrlRole
    };

    KPImageCollectionModel( QObject* parent = 0 );
    KPImageCollectionModel( KPQuickImageCollection* collection, QObject* parent = 0 );
    virtual ~KPImageCollectionModel() {}

    void setImageCollection(KPQuickImageCollection* collection);
    KPQuickImageCollection* imageCollection() const { return m_collection; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int,QByteArray> roleNames() const;

Q_SIGNALS:
    void imageCollectionChanged(KPQuickImageCollection*);

public:
    Q_PROPERTY(KPQuickImageCollection* imageCollection READ imageCollection WRITE setImageCollection NOTIFY imageCollectionChanged);

private:
    QPointer<KPQuickImageCollection> m_collection;
    QList<QUrl> m_images;
};

}

#endif

