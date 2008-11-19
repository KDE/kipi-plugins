/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-11-03
 * Description : calendar parameters.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

// Qt includes.

#include <QDate>

// KDE includes.

#include <KGlobal>
#include <KLocale>
#include <KCalendarSystem>

// Local includes.

#include "calsettings.h"

namespace KIPICalendarPlugin
{

QPointer<CalSettings> CalSettings::instance_;

CalSettings::CalSettings()
{
    year_ = KGlobal::locale()->calendar()->earliestValidDate().year() + 1;
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
    year_ = year;
}

int CalSettings::year() const
{
    return year_;
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
            float zoom = qMin((float)previewSize/params.paperWidth,
                            (float)previewSize/params.paperHeight);
            params.width  = (int)(params.paperWidth  * zoom);
            params.height = (int)(params.paperHeight * zoom);

            params.imgPos = CalParams::Top;
            break;
        }
        case CalParams::Left:
        {
            float zoom = qMin((float)previewSize/params.paperWidth,
                            (float)previewSize/params.paperHeight);
            params.width  = (int)(params.paperHeight  * zoom);
            params.height = (int)(params.paperWidth   * zoom);

            params.imgPos = CalParams::Left;
            break;
        }
        default:
        {
            float zoom = qMin((float)previewSize/params.paperWidth,
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

}  // NameSpace KIPICalendarPlugin
