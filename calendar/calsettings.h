/* ============================================================
 * File  : calsettings.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-04
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef CALSETTINGS_H
#define CALSETTINGS_H

// Qt includes.

#include <qmap.h>
#include <qstring.h>
#include <qfont.h>

// KDE includes.

#include <kprinter.h>
#include <kurl.h>

namespace KIPICalendarPlugin
{

class CalParams
{
public:

    enum ImagePosition
    {
        Top = 0,
        Left,
        Right
    };

    KPrinter::PageSize pageSize;
    int                paperWidth;
    int                paperHeight;
    int                width;
    int                height;
    bool               drawLines;
    float              ratio;
    ImagePosition      imgPos;
    QFont              baseFont;
};

class CalSettings
{
public:

    CalSettings();
    ~CalSettings();

    void    setYear(int year);
    int     getYear() const;
    void    setImage(int month, const KURL& url);
    KURL getImage(int month) const;

    CalParams calParams;

    static CalSettings* instance();

private:

    static CalSettings* instance_;

    QMap<int,KURL> monthMap_;
    int                   year_;

};

}  // NameSpace KIPICalendarPlugin

#endif /* CALSETTINGS_H */
