/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : GPS track list container.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSTRACKLISTCONTAINER_H
#define GPSTRACKLISTCONTAINER_H

// Qt includes

#include <QMap>
#include <QString>
#include <QDateTime>

// KDE includes

#include <kurl.h>

// Local includes

#include <gpsdatacontainer.h>

namespace KIPIGPSSyncPlugin
{

class GPSTrackListItem
{
public:

    GPSTrackListItem(): m_dirty(false), m_id(0), m_url(KUrl()), m_gpsData(GPSDataContainer())
    {};

    GPSTrackListItem(const KUrl& url, const GPSDataContainer gpsData)
                   : m_dirty(false), m_id(0), m_url(url), m_gpsData(gpsData)
    {};

    ~GPSTrackListItem()
    {};

    GPSTrackListItem& operator=(const GPSTrackListItem& data)
    {
        m_dirty    = data.isDirty();
        m_id       = data.id();
        m_url      = data.url();
        m_gpsData  = data.gpsData();
        return *this;
    };

    void setDirty(bool dirty)                        { m_dirty    = dirty;   };
    void setId(int id)                               { m_id       = id;      };
    void setUrl(const KUrl& url)                     { m_url      = url;     };
    void setGPSData(const GPSDataContainer& gpsData) { m_gpsData  = gpsData; };

    int       isDirty()  const       { return m_dirty;          };
    int       id()  const            { return m_id;             };
    KUrl      url() const            { return m_url;            };
    QString   fileName() const       { return m_url.fileName(); };
    GPSDataContainer gpsData() const { return m_gpsData;        };

private:

    bool             m_dirty;

    int              m_id;

    KUrl             m_url;

    GPSDataContainer m_gpsData;
};

typedef QMap<QDateTime, GPSTrackListItem> GPSTrackList;

} // namespace KIPIGPSSyncPlugin

#endif  // GPSTRACKLISTCONTAINER_H
