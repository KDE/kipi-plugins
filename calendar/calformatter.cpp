/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-11-23
 * Description : A class that decides which
 *               cell of the calendar should be painted with which color.
 *
 * Copyright (C) 2005 by Maciek Borowka <maciek_AT_borowka.net>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
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

#include "calformatter.h"
#include "calformatter.moc"

// Qt includes.

#include <QDateTime>
#include <QMap>

// KDE includes.

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kurl.h>

// LibKCAL includes.

#include <kcal/calendarlocal.h>

namespace KIPICalendarPlugin
{

class CalFormatter::Data
{

public:

    struct Day
    {
        Day() {}
        Day(QColor c, QString d): color(c), description(d) {}

        QColor color;
        QString description;
    };

    QMap<QDate, Day> special;
    int year_;

    Data(int year) : year_(year) {}

    // in case of two events on the same day, the first loaded event sets the color.
    void append(const QDate &dt, const Day &day)
    {
        if (special.contains(dt))
            special[dt].description.append("; ").append(day.description);
        else
            special[dt] = day;
    }

    void load(const KUrl &url, int year, const QColor color)
    {
        KCal::CalendarLocal calendar("UTC");

        if (!(url.isEmpty()))
        {
            kDebug(51000) << "Loading calendar from file " << url.fileName();
            if (calendar.load(url.fileName()))
            {
                QDate qFirst, qLast;
                KGlobal::locale()->calendar()->setYMD(qFirst, year, 1, 1);
                KGlobal::locale()->calendar()->setYMD(qLast, year + 1, 1, 1);
                qLast = qLast.addDays(-1);
                KDateTime dtFirst(qFirst);
                KDateTime dtLast(qLast);

                KCal::Event::List list = calendar.rawEvents(qFirst, qLast);
                KCal::Event::List::iterator it;
                KCal::Recurrence *recur;

                KDateTime dtCurrent;
                int counter = 0;
                for ( it = list.begin(); it != list.end(); ++it )
                {
                    kDebug(51000) << (*it)->summary() << endl << "--------" << endl;
                    counter++;
                    if ((*it)->recurs())
                    {
                        recur = (*it)->recurrence();
                        for (dtCurrent = recur->getNextDateTime(dtFirst.addDays(-1));
                             (dtCurrent <= dtLast) && dtCurrent.isValid();
                             dtCurrent = recur->getNextDateTime(dtCurrent))
                        {
                            append(dtCurrent.date(), Day(color, (*it)->summary()));
                        }
                    }
                    else
                    {
                        append((*it)->dtStart().date(), Day(color, (*it)->summary()));
                    }
                }
                kDebug(51000) << "Loaded " << counter << " events for year " << year << endl;
                calendar.close();
            }
        }
    };

};

CalFormatter::CalFormatter(int year, const KUrl& ohUrl,
                           const KUrl& fhUrl, QObject *parent)
            : QObject(parent)
{
    year_    = year;
    holidays = new Data(year);
    holidays->load(ohUrl, year_, Qt::red);
    holidays->load(fhUrl, year_, Qt::green);
}

CalFormatter::~CalFormatter()
{
    delete holidays;
}

bool CalFormatter::isPrayDay(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);
    return (dt.dayOfWeek() == KGlobal::locale()->calendar()->weekDayOfPray());
}

/*!
    \returns true if special formatting is to be applied to the particular day
 */
bool KIPICalendarPlugin::CalFormatter::isSpecial(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);

    return (isPrayDay(month,day) || holidays->special.contains(dt));
}


/*!
    \returns the color to be used for painting of the day info
 */
QColor KIPICalendarPlugin::CalFormatter::getDayColor(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);

    if (isPrayDay(month, day))
        return Qt::red;

    if (holidays->special.contains(dt))
        return holidays->special[dt].color;

    //default
    return Qt::black;
}


/*!
    \returns the description of the day to be painted on the calendar.
 */
QString KIPICalendarPlugin::CalFormatter::getDayDescr(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);

    QString ret;

    if (holidays->special.contains(dt))
        ret = holidays->special[dt].description;

    return ret;
}

} // namespace KIPICalendarPlugin
