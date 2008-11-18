/* ============================================================
 * File  : calformatter.h
 * Authors: Maciek Borowka <maciek_AT_borowka.net>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: A class that decides which
 *              cell of the calendar should be painted with which color.
 *
 * Copyright 2005 by Maciek Borowka
 * Copyright 2008 by Orgad Shaneh
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


#ifndef __CALFORMATTER_H__
#define __CALFORMATTER_H__

// Qt includes.

#include <QObject>

class KUrl;

namespace KIPICalendarPlugin {

/**
@author Maciek Borowka
*/
class CalFormatter : public QObject
{
public:

    CalFormatter(int year, const KUrl &ohUrl, const KUrl &fhUrl, QObject *parent = 0);
    ~CalFormatter();

    bool isSpecial(int month, int day);

    QColor getDayColor(int month, int day);
    QString getDayDescr(int month, int day);

protected:
    int year_;

    bool isPrayDay(int month, int day);

    class Data;
    Data *holidays;
};

}

#endif // __CALFORMATTER_H__
