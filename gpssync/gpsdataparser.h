/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : GPS data file parser.
 *               (GPX format http://www.topografix.com/gpx.asp).
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSDATAPARSER_H
#define GPSDATAPARSER_H

// Qt includes

#include <QDateTime>
#include <QMap>

// KDE includes

#include <kurl.h>

// Local includes

#include "gpsdatacontainer.h"

namespace KIPIGPSSyncPlugin
{

class GPSDataParser
{

public:

    GPSDataParser();
    ~GPSDataParser(){};	

    bool loadGPXFile(const KUrl& url);

    void clear();
    int  numPoints();
    bool matchDate(const QDateTime& photoDateTime, int maxGapTime, int timeZone, 
                   bool interpolate, int interpolationDstTime, 
                   GPSDataContainer& gpsData);

private:

    // Methods used to perform interpolation.
    QDateTime findNextDate(const QDateTime& dateTime, int secs);
    QDateTime findPrevDate(const QDateTime& dateTime, int secs);

protected:

    typedef QMap<QDateTime, GPSDataContainer> GPSDataMap; 

    GPSDataMap m_GPSDataMap;
};

} // namespace KIPIGPSSyncPlugin

#endif  // GPSDATAPARSER_H
