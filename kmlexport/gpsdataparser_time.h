/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-01-17
 * Description : A function to parse time strings to QDateTime.
 *               Put into an extra file for easier testing.
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

#ifndef GPSDATAPARSER_TIME_H
#define GPSDATAPARSER_TIME_H

// Qt includes

#include <QDateTime>

namespace KIPIKMLExportPlugin
{

QDateTime GPSDataParserParseTime(QString timeString)
{
    if (timeString.isEmpty())
    {
        return QDateTime();
    }

    // we want to be able to parse these formats:
    // "2010-01-14T09:26:02.287-02:00" <-- here we have to cut off the -02:00 and replace it with 'Z'
    // "2010-01-14T09:26:02.287+02:00" <-- here we have to cut off the +02:00 and replace it with 'Z'
    // "2009-03-11T13:39:55.622Z"

    const int timeStringLength      = timeString.length();
    const int timeZoneSignPosition  = timeStringLength-6;

    // does the string contain a timezone offset?
    int timeZoneOffsetSeconds       = 0;
    const int timeZonePlusPosition  = timeString.lastIndexOf("+");
    const int timeZoneMinusPosition = timeString.lastIndexOf("-");

    if ( (timeZonePlusPosition  == timeZoneSignPosition)   ||
         (timeZoneMinusPosition == timeZoneSignPosition) )
    {
        const int timeZoneSign       = (timeZonePlusPosition == timeZoneSignPosition) ? +1 : -1;

        // cut off the last digits:
        const QString timeZoneString = timeString.right(6);
        timeString.chop(6);
        timeString += 'Z';

        // determine the time zone offset:
        bool okayHour          = false;
        bool okayMinute        = false;
        const int hourOffset   = timeZoneString.mid(1, 2).toInt(&okayHour);
        const int minuteOffset = timeZoneString.mid(4, 2).toInt(&okayMinute);

        if (okayHour && okayMinute)
        {
            timeZoneOffsetSeconds =  hourOffset*3600 + minuteOffset*60;
            timeZoneOffsetSeconds *= timeZoneSign;
        }
    }

    QDateTime theTime = QDateTime::fromString(timeString, Qt::ISODate);
    theTime           = theTime.addSecs(-timeZoneOffsetSeconds);

    return theTime;
}

} /* KIPIKMLExportPlugin */

#endif /* GPSDATAPARSER_TIME_H */
