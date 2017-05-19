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
#include "kpimageinfo.h"

using namespace KIPIPlugins
using namespace KIPI

KPImageCollectionModel::KPImageCollectionModel(QObject* parent) :
	QAbstractListModel(parent)
{
}

KPImageCollectionModel( const KIPI::ImageCollection& collection, QObject* parent) :
	QAbstractListModel(parent), m_collection(collection)
{
}

void KPImageCollectionModel::setImageCollection( const KIPI::ImageCollection& collection )
{
	beginResetModel();
	m_collection = collection;
	endResetModel();
}

int KPImageCollectionModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid()) {
		return 0;
	}

	return m_collection.urls().size();
}

QVariant KPImageCollectionModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid()) {
		return QVariant();
	}
	if(index.column() > 0) {
		return QVariant();
	}
	if(index.row() < 0 || index.row() >= m_collection.urls().size()) {
		return QVariant();
	}

	const QUrl& url = m_collection.urls()[index.row()];
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
}

