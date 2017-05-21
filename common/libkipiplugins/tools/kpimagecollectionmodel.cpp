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

#include "kpimagecollectionmodel.h"

// Qt includes

#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <QObject>
// Local includes

#include "kipiplugins_export.h"
#include "kpimagecollectionmodel.h"
#include "kpquickglobal.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;
using namespace KIPI;

KPImageCollectionModel::KPImageCollectionModel(QObject* parent) :
    QAbstractListModel(parent)
{
}

KPImageCollectionModel::KPImageCollectionModel( KPQuickImageCollection* collection, QObject* parent) :
    QAbstractListModel(parent), m_collection(collection)
{
}

void KPImageCollectionModel::setImageCollection( KPQuickImageCollection* collection )
{
    beginResetModel();
    m_collection = collection;
    if(m_collection != 0) {
        m_images = m_collection->images();
    }
    endResetModel();
    emit imageCollectionChanged(m_collection);
}

int KPImageCollectionModel::rowCount(const QModelIndex &parent) const
{
    if(m_collection == 0 || !m_collection->isValid()) {
        return 0;
    }
    if(parent.isValid()) {
        return 0;
    }

    return m_images.size();
}

QVariant KPImageCollectionModel::data(const QModelIndex &index, int role) const
{
    int pos = index.row();

    if(!index.isValid()) {
        return QVariant();
    }
    if(index.column() > 0) {
        return QVariant();
    }
    if(m_collection == 0 || !m_collection->isValid()) {
        return QVariant();
    }
    if(pos < 0 || pos >= m_images.size()) {
        return QVariant();
    }


    // qCDebug(KIPIPLUGINS_LOG) << "Requesting at: " << pos << " total count: " << m_collection->images().size(); // TODO: REMOVE
    QUrl url = m_images.at(pos);
    // qCDebug(KIPIPLUGINS_LOG) << "Model::data url: " << url; // TODO: REMOVE
    switch(role) {
        case Qt::DisplayRole:
        case UrlRole:
            return url;
        case ThumbnailUrlRole:
            return createThumbnailUrl(url);
        case PreviewUrlRole:
            return createPreviewUrl(url);
        default:
            return QVariant();
    }

    return QVariant();
}

QHash<int,QByteArray> KPImageCollectionModel::roleNames() const
{
    QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
    result[UrlRole] = "url";
    result[ThumbnailUrlRole] = "thumbnailUrl";
    result[PreviewUrlRole] = "previewUrl";

    return result;
}

