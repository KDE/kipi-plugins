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

// WMW2 includes

#include <worldmapwidget2/worldmapwidget2_primitives.h>

// local includes

#include "kipiimageitem.h"

namespace KIPIGPSSyncPlugin
{

class GPSImageItem : public KipiImageItem
{
public:
    static const int RoleCoordinates = Qt::UserRole + 1;

    static const int ColumnLatitude = 2;
    static const int ColumnLongitude = 3;
    static const int ColumnAltitude = 4;

    GPSImageItem(KIPI::Interface* const interface, const KUrl& url, const bool autoLoad = true);
    virtual ~GPSImageItem();

    void setCoordinates(const WMW2::WMWGeoCoordinate& newCoordinates);
    inline WMW2::WMWGeoCoordinate coordinates() const { return m_coordinates; };

    static void setHeaderData(KipiImageModel* const model);

protected:
    // these are only to be called by the KipiImageModel
    virtual QVariant data(const int column, const int role) const;
    virtual bool setData(const int column, const int role, const QVariant& value);
    virtual void loadImageDataInternal();

protected:
    WMW2::WMWGeoCoordinate m_coordinates;
};

} /* KIPIGPSSyncPlugin */

#endif /* GPSIMAGEITEM_H */

