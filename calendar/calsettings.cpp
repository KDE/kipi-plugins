/* ============================================================
 * File  : calsettings.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-04
 * Description :
 *
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

// KDE includes.

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>

// Local includes.

#include "calsettings.h"

namespace KIPICalendarPlugin
{

CalSettings* CalSettings::instance_ = 0;

CalSettings::CalSettings()
{
    instance_ = this;
    year_ = KGlobal::locale()->calendar()->minValidYear() + 1;
}

CalSettings::~CalSettings()
{
    instance_ = 0;
}

CalSettings* CalSettings::instance()
{
    return instance_;
}

void CalSettings::setYear(int year)
{
    year_ = year;
}

int CalSettings::getYear() const
{
    return year_;
}

void CalSettings::setImage(int month, const KURL& path)
{
    monthMap_.insert(month, path, true);
}

KURL CalSettings::getImage(int month) const
{
    return monthMap_.contains(month) ? monthMap_[month] : KURL();
}

}  // NameSpace KIPICalendarPlugin

