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

#ifndef KIPIIMAGEITEM_H
#define KIPIIMAGEITEM_H

// Qt includes

#include <QVariant>

// KDE includes

#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Libkmap includes

#include <libkmap/kmap_primitives.h>

// local includes

#include "gpsdatacontainer.h"
#include "kdebug.h"
#include "kmessagebox.h"

namespace KExiv2Iface
{
    class KExiv2;
}

namespace KIPIGPSSyncPlugin
{

class RGInfo {

    public:

    RGInfo()
    :id(),
     coordinates(),
     rgData(){   }

    QPersistentModelIndex id;
    KMapIface::WMWGeoCoordinate coordinates;
    QMap<QString, QString> rgData;
};


enum Type {
    TypeChild = 1,
    TypeSpacer = 2,
    TypeNewChild = 4
};

typedef struct TagData
{
    QString tagName;
    Type tagType;

} TagData;


class KipiImageModel;

class KipiImageItem
{
public:

    static const int RoleCoordinates = Qt::UserRole + 1;

    static const int ColumnThumbnail = 0;
    static const int ColumnFilename = 1;
    static const int ColumnDateTime = 2;
    static const int ColumnLatitude = 3;
    static const int ColumnLongitude = 4;
    static const int ColumnAltitude = 5;
    static const int ColumnAccuracy = 6;
    static const int ColumnTags = 7;
    static const int ColumnStatus = 8;
    static const int ColumnHDOP = 9;
    static const int ColumnPDOP = 10;
    static const int ColumnFixType = 11;
    static const int ColumnNSatellites = 12;

    static const int ColumnGPSImageItemCount = 13;

    KipiImageItem(KIPI::Interface* const interface, const KUrl& url, const bool autoLoad = true);
    virtual ~KipiImageItem();

    void setUrl(const KUrl& url);
    inline KUrl url() const { return m_url; };

    inline QDateTime dateTime() const { return m_dateTime; };

    static void setHeaderData(KipiImageModel* const model);
    bool lessThan(const KipiImageItem* const otherItem, const int column) const;

    void setCoordinates(const KMapIface::WMWGeoCoordinate& newCoordinates);
    inline KMapIface::WMWGeoCoordinate coordinates() const { return m_gpsData.getCoordinates(); }
    inline GPSDataContainer gpsData() const { return m_gpsData; }
    inline void setGPSData(const GPSDataContainer& container) { m_gpsData = container; m_dirty = true; emitDataChanged(); }
    void restoreGPSData(const GPSDataContainer& container);
    inline bool isDirty() const { return m_dirty; }

    inline void setTagList(const QList<QList<TagData> >& externalTagList) { m_tagList = externalTagList; m_tagListDirty = true; emitDataChanged(); };
    inline bool isTagListDirty() const { return m_tagListDirty; }
    inline QList<QList<TagData> > getTagList() const { return m_tagList; };
    void restoreRGTagList(const QList<QList<TagData> >&);
    void writeTagsToXmp(const bool writeXmpTags) { m_writeXmpTags = writeXmpTags; }

    
    QString saveChanges();

protected:
    // these are only to be called by the KipiImageModel
    QVariant data(const int column, const int role) const;
    void setModel(KipiImageModel* const model);
    void emitDataChanged();
    void loadImageData();
    KExiv2Iface::KExiv2* getExiv2ForFile();

protected:
    KIPI::Interface* m_interface;
    KipiImageModel* m_model;

    KUrl m_url;
    QDateTime m_dateTime;

    bool m_dirty;
    GPSDataContainer m_gpsData;
    GPSDataContainer m_savedState;

    bool m_tagListDirty;
    QList<QList<TagData> > m_tagList;
    QList<QList<TagData> > m_savedTagList;
    bool m_writeXmpTags;

    friend class KipiImageModel;
};

} /* KIPIGPSSyncPlugin */

#endif /* KIPIIMAGEITEM_H */

