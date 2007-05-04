/* ============================================================
 * File  : calformatter.h
 * Author: Maciek Borowka <maciek_AT_borowka.net>
 * Date  : 2005-11-23
 * Description : The declaration of a class that decides which
 * cell of the calendar should be painted with which color.
 *
 * Copyright 2005 by Maciek Borowka
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


#ifndef KIPICALENDARPLUGINCALFORMATTER_H
#define KIPICALENDARPLUGINCALFORMATTER_H

#include <qobject.h>

namespace KIPICalendarPlugin {

/**
@author Maciek Borowka
*/
class CalFormatter : public QObject
{
Q_OBJECT
public:

    CalFormatter();
    ~CalFormatter();

    bool isSpecial(int month, int day);

    QColor getDayColor(int month, int day);
    QString getDayDescr(int month, int day);

    void init(int year, const QString & ohFile, const QString & fhFile);

protected:
    int year_;

    bool isPrayDay(int month, int day);

    bool initialized;

    class Data;
    Data *d;
};

}

#endif
