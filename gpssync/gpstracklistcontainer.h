/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : GPS track list container.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QList>
#include <QString>
#include <QDateTime>

// KDE includes.

#include <kurl.h>

// Local includes.

#include <gpsdatacontainer.h>

namespace KIPIGPSSyncPlugin
{

class GPSTrackListItem
{
public:

    GPSTrackListItem(int id, const KUrl& url, const QDateTime& dt, const GPSDataContainer gpsData)
                   : m_id(id), m_dateTime(dt), m_url(url), m_gpsData(gpsData)
    {};

    ~GPSTrackListItem()
    {};

    GPSTrackListItem& operator=(const GPSTrackListItem& data)
    {
        m_id       = data.id();
        m_dateTime = data.dateTime();
        m_url      = data.url();
        m_gpsData  = data.gpsData();
        return *this;
    };

    void setId(int id)                               { m_id       = id;      };
    void setDateTime(const QDateTime& dt)            { m_dateTime = dt;      };
    void setUrl(const KUrl& url)                     { m_url      = url;     };
    void setGPSData(const GPSDataContainer& gpsData) { m_gpsData  = gpsData; };

    int       id()  const            { return m_id;             };
    QDateTime dateTime() const       { return m_dateTime;       };
    KUrl      url() const            { return m_url;            };
    QString   fileName() const       { return m_url.fileName(); };
    GPSDataContainer gpsData() const { return m_gpsData;        };
    
private:

    int              m_id;

    QDateTime        m_dateTime;
    
    KUrl             m_url;

    GPSDataContainer m_gpsData;
};

typedef QList<GPSTrackListItem> GPSTrackList;

} // NameSpace KIPIGPSSyncPlugin

#endif  // GPSTRACKLISTCONTAINER_H
