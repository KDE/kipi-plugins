/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-11-03
 * Description : calendar parameters.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
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

#include "calsettings.h"
#include "calsettings.moc"

// KDE includes

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>

// LibKCAL includes

#include <kcal/calendarlocal.h>

namespace KIPICalendarPlugin
{

QPointer<CalSettings> CalSettings::instance_;

CalSettings::CalSettings()
{
    params.year = KGlobal::locale()->calendar()->earliestValidDate().year() + 1;
    setPaperSize("A4");
    setImagePos(0);
}

CalSettings::~CalSettings()
{
}

CalSettings* CalSettings::instance()
{
    if (instance_.isNull())
      instance_ = new CalSettings();
    return instance_;
}

void CalSettings::setYear(int year)
{
    params.year = year;
}

int CalSettings::year() const
{
    return params.year;
}

void CalSettings::setImage(int month, const KUrl& path)
{
    monthMap_.insert(month, path);
}

KUrl CalSettings::image(int month) const
{
    return monthMap_.contains(month) ? monthMap_[month] : KUrl();
}

void CalSettings::setPaperSize(const QString &paperSize)
{
    if (paperSize == "A4")
    {
        params.paperWidth  = 210;
        params.paperHeight = 297;
        params.pageSize    = QPrinter::A4;
    }
    else if (paperSize == "US Letter")
    {
        params.paperWidth  = 216;
        params.paperHeight = 279;
        params.pageSize    = QPrinter::Letter;
    }
    emit settingsChanged();
}

void CalSettings::setImagePos(int pos)
{
    const int previewSize = 300;

    switch (pos)
    {
        case CalParams::Top:
        {
            float zoom    = qMin((float)previewSize/params.paperWidth,
                                 (float)previewSize/params.paperHeight);
            params.width  = (int)(params.paperWidth  * zoom);
            params.height = (int)(params.paperHeight * zoom);

            params.imgPos = CalParams::Top;
            break;
        }
        case CalParams::Left:
        {
            float zoom    = qMin((float)previewSize/params.paperWidth,
                                 (float)previewSize/params.paperHeight);
            params.width  = (int)(params.paperHeight  * zoom);
            params.height = (int)(params.paperWidth   * zoom);

            params.imgPos = CalParams::Left;
            break;
        }
        default:
        {
            float zoom    = qMin((float)previewSize/params.paperWidth,
                                 (float)previewSize/params.paperHeight);
            params.width  = (int)(params.paperHeight  * zoom);
            params.height = (int)(params.paperWidth   * zoom);

            params.imgPos = CalParams::Right;
        }
    }
    emit settingsChanged();
}

void CalSettings::setDrawLines(bool draw)
{
    if (params.drawLines != draw)
    {
        params.drawLines = draw;
        emit settingsChanged();
    }
}

void CalSettings::setRatio(int ratio)
{
    if (params.ratio != ratio)
    {
        params.ratio = ratio;
        emit settingsChanged();
    }
}

void CalSettings::setFont(const QString &font)
{
    if (params.baseFont.family() != font)
    {
        params.baseFont = QFont(font);
        emit settingsChanged();
    }
}

void CalSettings::clearSpecial()
{
    special.clear();
}

void CalSettings::addSpecial(const QDate &date, const Day &info)
{
    if (special.contains(date))
        special[date].second.append("; ").append(info.second);
    else
        special[date] = info;
}

void CalSettings::loadSpecial(const KUrl &url, const QColor &color)
{
    KCal::CalendarLocal calendar("UTC");

    if (!(url.isEmpty()))
    {
        kDebug() << "Loading calendar from file " << url.path();
        if (!calendar.load(url.path()))
        {
            kDebug() << "Failed!";
        }
        else
        {
            QDate qFirst, qLast;
            KGlobal::locale()->calendar()->setYMD(qFirst, params.year, 1, 1);
            KGlobal::locale()->calendar()->setYMD(qLast, params.year + 1, 1, 1);
            qLast = qLast.addDays(-1);
            KDateTime dtFirst(qFirst);
            KDateTime dtLast(qLast);
            KDateTime dtCurrent;

            int counter = 0;
            KCal::Event::List list = calendar.rawEvents(qFirst, qLast);
            foreach ( KCal::Event *event, list )
            {
                kDebug() << event->summary() << endl << "--------";
                counter++;
                if (event->recurs())
                {
                    KCal::Recurrence *recur = event->recurrence();
                    for (dtCurrent = recur->getNextDateTime(dtFirst.addDays(-1));
                         (dtCurrent <= dtLast) && dtCurrent.isValid();
                         dtCurrent = recur->getNextDateTime(dtCurrent))
                    {
                        addSpecial(dtCurrent.date(), Day(color, event->summary()));
                    }
                }
                else
                {
                    addSpecial(event->dtStart().date(), Day(color, event->summary()));
                }
            }
            kDebug() << "Loaded " << counter << " events";
            calendar.close();
        }
    }
}

bool CalSettings::isPrayDay(const QDate &date) const
{
    return (date.dayOfWeek() == KGlobal::locale()->calendar()->weekDayOfPray());
}

/*!
    \returns true if special formatting is to be applied to the particular day
 */
bool CalSettings::isSpecial(int month, int day) const
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, params.year, month, day);

    return (isPrayDay(dt) || special.contains(dt));
}

/*!
    \returns the color to be used for painting of the day info
 */
QColor CalSettings::getDayColor(int month, int day) const
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, params.year, month, day);

    if (isPrayDay(dt))
        return Qt::red;

    if (special.contains(dt))
        return special[dt].first;

    //default
    return Qt::black;
}

/*!
    \returns the description of the day to be painted on the calendar.
 */
QString CalSettings::getDayDescr(int month, int day) const
{
    QDate dt;
    KGlobal::locale()->calendar()->setYMD(dt, params.year, month, day);

    return special[dt].second;
    QString ret;

    if (special.contains(dt))
        ret = special[dt].second;

    return ret;
}

}  // NameSpace KIPICalendarPlugin
