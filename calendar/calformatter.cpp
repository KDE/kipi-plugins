/* ============================================================
 * File  : calformatter.cpp
 * Author: Maciek Borowka <maciek_AT_borowka.net>
 * Date  : 2005-11-23
 * Description : The implementation of a class that decides which
 * cell of the calendar should be painted with which color.
 *
 * Copyright 2005 by Maciek Borowka
 * Revised by Orgad Shaneh, 2007
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <qdatetime.h>
#include <qcolor.h>
#include <qmap.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <libkcal/calendarlocal.h>

#include "calformatter.h"


namespace KIPICalendarPlugin {


class CalFormatter::Data
{
public:
    class Day {
    public:
        Day(){};
        Day(QColor c, QString d): color(c), description(d) {};

        QColor color;
        QString description;
     };


    QString ohFile;
    QString fhFile;

    QMap<QDate, Day> oh;//official holidays
    QMap<QDate, Day> fh;//family holidays

    Data()
    {

//you define a holiday that way:
oh[QDate(2005, 1, 1)] = Day(Qt::red, "New year!");
//oh[QDate(2005, 1, 4)] = Day(Qt::red, "Fete Nat");


fh[QDate(2005,1,3)] = Day(Qt::green, "Adam");
//fh[QDate(2005, 1, 14)] = Day(Qt::green, "Maciek");


    };

};

CalFormatter::CalFormatter() : initialized(false)
{

}


CalFormatter::~CalFormatter()
{
}

void CalFormatter::init(int year, const QString & ohFile, const QString & fhFile)
{
    if (initialized)
        return;

    d = new Data();
    year_ = year;
    d->ohFile = ohFile;
    d->fhFile = fhFile;

    kdDebug(51000) << "Loading calendar from file" << endl;
    KCal::CalendarLocal * calendar;

    if (not(ohFile.isEmpty())) {
        calendar = new KCal::CalendarLocal("UTC");
        if (calendar->load(ohFile)) {
                QDate dtFirst, dtLast;
                KGlobal::locale()->calendar()->setYMD(dtFirst, year_, 1, 1);
                KGlobal::locale()->calendar()->setYMD(dtLast, year_ + 1, 1, 1);
                dtLast = dtLast.addDays(-1);

                KCal::Event::List list = calendar->rawEvents(dtFirst, dtLast);
                KCal::Event::List::iterator it;
		KCal::Recurrence *recur;

		QDateTime dtCurrent;
                int counter = 0;
                for ( it = list.begin(); it != list.end(); ++it )
                {
                        kdDebug(51000) << (*it)->summary() << endl << "--------" << endl;
                        counter++;
			if ((*it)->doesRecur())
			{
				recur = (*it)->recurrence();
				for (dtCurrent = recur->getNextDateTime(dtFirst); (dtCurrent < dtLast) && dtCurrent.isValid(); dtCurrent = recur->getNextDateTime(dtCurrent))
				{
					kdDebug(51000) << dtCurrent.toString() << endl;
					d->oh[dtCurrent.date()] = CalFormatter::Data::Day(Qt::red, (*it)->summary());
				}
				
			}
			else
	                        d->oh[(*it)->dtStart().date()] = CalFormatter::Data::Day(Qt::red, (*it)->summary());
                }
                kdDebug(51000) << "Loaded " << counter << " events for year " << year_ << endl;
        }
        delete calendar;
    }

    if (not(fhFile.isEmpty())) {
        calendar = new KCal::CalendarLocal("UTC");
        if (calendar->load(fhFile)) {

                KCal::Event::List list = calendar->rawEvents(QDate(year_,1,1), QDate(year_,12,31));
                KCal::Event::List::iterator it;

                QString eventDate;
                int counter = 0;
                for ( it = list.begin(); it != list.end(); ++it )
                {
                        counter++;
                        d->fh[(*it)->dtStart().date()] = CalFormatter::Data::Day(Qt::red, (*it)->summary());
                        //kdDebug(51000) << eventDate << "----" <<  (*it)->summary() << endl;
                }
                kdDebug(51000) << "Loaded " << counter << " events for year " << year_ << endl;
        }
        delete calendar;
    }
}

bool CalFormatter::isPrayDay(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);
    return (dt.dayOfWeek() == KGlobal::locale()->calendar()->weekDayOfPray());
}

/*!
    Returns true if special formatting is to be applied to the particular day
 */
bool KIPICalendarPlugin::CalFormatter::isSpecial(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);

    return (isPrayDay(month,day) || d->oh.contains(dt) || d->fh.contains(dt));
}


/*!
    Returns the color to be used for painting of the day info
 */
QColor KIPICalendarPlugin::CalFormatter::getDayColor(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);

    if (isPrayDay(month, day))
        return Qt::red;

    if (d->oh.contains(dt))
        return Qt::red;

    if (d->fh.contains(dt))
        return Qt::green;

    //default
    return Qt::black;
}


/*!
    Returns the description of the day to be painted on the calendar.
 */
QString KIPICalendarPlugin::CalFormatter::getDayDescr(int month, int day)
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, year_, month, day);

    QString ret;

    if (d->oh.contains(dt))
        ret = d->oh[dt].description;

    if (d->fh.contains(dt)) {
        if (ret.isNull()) 
            return d->fh[dt].description;
        else
            return ret.append(";").append(d->fh[dt].description);
    }

    return ret;
}
}
