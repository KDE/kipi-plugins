/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-09-19
 * Description : GPS data container.
 *
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSDATACONTAINER_H
#define GPSDATACONTAINER_H

#include <QStringList>

namespace KIPIKMLExportPlugin
{

class GPSDataContainer
{
public:

    GPSDataContainer()
        : m_interpolated(false),
          m_altitude(0.0),
          m_latitude(0.0),
          m_longitude(0.0)
    {
    };

    GPSDataContainer(double altitude, double latitude,
                     double longitude, bool interpolated)
        : m_interpolated(interpolated),
          m_altitude(altitude),
          m_latitude(latitude),
          m_longitude(longitude)
    {
    };

    ~GPSDataContainer()
    {
    };

    GPSDataContainer& operator=(const GPSDataContainer& data)
    {
        m_interpolated = data.isInterpolated();
        m_altitude     = data.altitude();
        m_latitude     = data.latitude();
        m_longitude    = data.longitude();
        return *this;
    };

    // use this instead of '==', because '==' implies having the
    // same value for m_interpolated
    bool sameCoordinatesAs(const GPSDataContainer& a) const
    {
        return ( a.m_altitude == m_altitude ) &&
               ( a.m_latitude == m_latitude ) &&
               ( a.m_longitude == m_longitude);
    }

    void setInterpolated(bool ite)  { m_interpolated = ite; };
    void setAltitude(double alt)    { m_altitude     = alt; };
    void setLatitude(double lat)    { m_latitude     = lat; };
    void setLongitude(double lng)   { m_longitude    = lng; };

    bool   isInterpolated() const   { return m_interpolated; };
    double altitude()       const   { return m_altitude;     };
    double latitude()       const   { return m_latitude;     };
    double longitude()      const   { return m_longitude;    };

    QString altitudeString()  const { return QString::number(m_altitude,  'g', 12); }
    QString latitudeString()  const { return QString::number(m_latitude,  'g', 12); }
    QString longitudeString() const { return QString::number(m_longitude, 'g', 12); }

    QString geoUrl() const
    {
        return QString::fromLatin1("geo:%1,%2,%3")
                                   .arg(latitudeString())
                                   .arg(longitudeString())
                                   .arg(altitudeString());
    }

    static GPSDataContainer fromGeoUrl(const QString& url, bool* const parsedOkay)
    {
        // parse geo:-uri according to (only partially implemented):
        // http://tools.ietf.org/html/draft-ietf-geopriv-geo-uri-04
        // TODO: verify that we follow the spec fully!
        if (!url.startsWith("geo:"))
        {
            // TODO: error
            if (parsedOkay)
                *parsedOkay = false;

            return GPSDataContainer();
        }

        const QStringList parts = url.mid(4).split(',');

        GPSDataContainer position;

        if ((parts.size() == 3) || (parts.size() == 2))
        {
            bool okay          = true;
            double ptLongitude = 0.0;
            double ptLatitude  = 0.0;
            double ptAltitude  = 0.0;
            ptLatitude         = parts[0].toDouble(&okay);

            if (okay)
                ptLongitude = parts[1].toDouble(&okay);

            if (okay&&(parts.size()==3))
                ptAltitude = parts[2].toDouble(&okay);

            if (!okay)
            {
                *parsedOkay = false;
                return GPSDataContainer();
            }

            position = GPSDataContainer(ptAltitude, ptLatitude, ptLongitude, false);
        }
        else
        {
            if (parsedOkay)
                *parsedOkay = false;

            return GPSDataContainer();
        }

        if (parsedOkay)
                *parsedOkay = true;

        return position;
    }

private:

    bool   m_interpolated;

    double m_altitude;
    double m_latitude;
    double m_longitude;
};

} // namespace KIPIKMLExportPlugin

Q_DECLARE_METATYPE(KIPIKMLExportPlugin::GPSDataContainer)

#endif  // GPSDATACONTAINER_H
