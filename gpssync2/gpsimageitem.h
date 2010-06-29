/* ============================================================
 *
 * Date        : 2010-03-21
 * Description : An item to hold information about an image
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSIMAGEITEM_H
#define GPSIMAGEITEM_H

// Libkmap includes

#include <libkmap/kmap_primitives.h>

// local includes

#include "gpsdatacontainer.h"
#include "kipiimageitem.h"
#include "kdebug.h"
#include "kmessagebox.h"

namespace KIPIGPSSyncPlugin
{

class RGInfo {

    public:

    RGInfo()
    :id(),
     coordinates(),
     rgData(){   }


    //QVariant id;
    QPersistentModelIndex id;
    KMapIface::WMWGeoCoordinate coordinates;
    QMap<QString, QString> rgData;
};


class GPSImageItem : public KipiImageItem
{
public:
    static const int RoleCoordinates = Qt::UserRole + 1;
    static const int RoleTags = Qt::UserRole + 2;

    static const int ColumnLatitude = ColumnKipiImageItemCount + 0;
    static const int ColumnLongitude = ColumnKipiImageItemCount + 1;
    static const int ColumnAltitude = ColumnKipiImageItemCount + 2;
    static const int ColumnAccuracy = ColumnKipiImageItemCount + 3;
    static const int ColumnHDOP = ColumnKipiImageItemCount + 4;
    static const int ColumnPDOP = ColumnKipiImageItemCount + 5;
    static const int ColumnFixType = ColumnKipiImageItemCount + 6;
    static const int ColumnNSatellites = ColumnKipiImageItemCount + 7;
    static const int ColumnStatus = ColumnKipiImageItemCount + 8;
    static const int ColumnTags = ColumnKipiImageItemCount + 9;

    static const int ColumnGPSImageItemCount = ColumnKipiImageItemCount + 10;

    GPSImageItem(KIPI::Interface* const interface, const KUrl& url, const bool autoLoad = true);
    virtual ~GPSImageItem();

    void setCoordinates(const KMapIface::WMWGeoCoordinate& newCoordinates);
    inline KMapIface::WMWGeoCoordinate coordinates() const { return m_gpsData.getCoordinates(); }
    inline GPSDataContainer gpsData() const { return m_gpsData; }
    inline void setGPSData(const GPSDataContainer& container) { m_gpsData = container; m_dirty = true; emitDataChanged(); }
    inline void setTagInfo(QString rgTag){ tags = rgTag; emitDataChanged(); }
    void restoreGPSData(const GPSDataContainer& container);
    inline bool isDirty() const { return m_dirty; }

    static void setHeaderData(KipiImageModel* const model);
    QString saveChanges();
    virtual bool lessThan(const KipiImageItem* const otherItem, const int column) const;

protected:
    // these are only to be called by the KipiImageModel
    virtual QVariant data(const int column, const int role) const;
    virtual bool setData(const int column, const int role, const QVariant& value);
    virtual void loadImageDataInternal();

protected:
    GPSDataContainer m_gpsData;
    GPSDataContainer m_savedState;
    bool m_dirty;
    QString tags;
};

} /* KIPIGPSSyncPlugin */

#endif /* GPSIMAGEITEM_H */

