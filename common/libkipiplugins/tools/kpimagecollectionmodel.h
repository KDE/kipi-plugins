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

class KIPIPLUGINS_EXPORT KPImageCollectionMOdel : public QAbstractListModel
{
    Q_OBJECT

public:
	enum ImageRoles {
		UrlRole = Qt::UserRole + 1,
		ThumbnailUrlRole,
		PreviewUrlRole
	}

    KPImageCollectionModel( QObject* parent = 0 );
    KPImageCollectionModel( const KIPI::ImageCollection& collection, QObject* parent = 0 );
    virtual ~KPImageCollectionModel() {}

	void setImageCollection( const KIPI::ImageCollection& collection );
	const KIPI::ImageCOllection& imageCollection() const { return m_collection; }

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QHash<int,QByteArray> roleNames() const;

Q_SIGNALS:
	void imageCollectionChanged(const KIPI::ImageCollection& )
public:
	Q_PROPERTY(KIPI::ImageCollection imageCollection READ imageCollection NOTIFY imageCollectionChanged);

private:
	KIPI::ImageCollection m_collection;

};

}

#endif

