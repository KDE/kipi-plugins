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

namespace KExiv2Iface
{
    class KExiv2;
}

namespace KIPIGPSSyncPlugin
{

class KipiImageModel;

class KipiImageItem
{
public:

    static const int ColumnThumbnail = 0;
    static const int ColumnFilename = 1;
    static const int ColumnDateTime = 2;
    static const int ColumnKipiImageItemCount = 3;

    KipiImageItem(KIPI::Interface* const interface, const KUrl& url, const bool autoLoad = true);
    virtual ~KipiImageItem();

    virtual void setUrl(const KUrl& url);
    inline KUrl url() const { return m_url; };

    inline QDateTime dateTime() const { return m_dateTime; };

    static void setHeaderData(KipiImageModel* const model);
    virtual bool lessThan(const KipiImageItem* const otherItem, const int column) const;

protected:
    // these are only to be called by the KipiImageModel
    virtual QVariant data(const int column, const int role) const;
    virtual bool setData(const int column, const int role, const QVariant& value);
    void setModel(KipiImageModel* const model);
    void emitDataChanged();
    void loadImageData();
    virtual void loadImageDataInternal();
    void openExiv2IFaceIfNotOpen(const bool saveOnClose);
    void closeExiv2IFaceIfOpen();

protected:
    KIPI::Interface* m_interface;
    KUrl m_url;
    QDateTime m_dateTime;
    KipiImageModel* m_model;
    KExiv2Iface::KExiv2* m_exiv2Iface;
    bool m_exiv2IfaceSaveOnClose;

    friend class KipiImageModel;
};

} /* KIPIGPSSyncPlugin */

#endif /* KIPIIMAGEITEM_H */

