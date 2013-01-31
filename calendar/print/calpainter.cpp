/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : painter class to draw calendar.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
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

#include "calpainter.moc"

// Qt includes

#include <QDate>
#include <QFileInfo>
#include <QMatrix>
#include <QPaintDevice>
#include <QRect>
#include <QString>

// KDE includes

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "calsettings.h"

namespace KIPICalendarPlugin
{

CalPainter::CalPainter(QPaintDevice* const pd)
    : QPainter(pd)
{
    orientation_ = KPMetadata::ORIENTATION_UNSPECIFIED;
    cancelled_   = false;
}

CalPainter::~CalPainter()
{
}

void CalPainter::cancel()
{
    cancelled_ = true;
}

void CalPainter::setImage(const KUrl& imagePath, KPMetadata::ImageOrientation orientation)
{
    imagePath_   = imagePath;
    orientation_ = orientation;
}

void CalPainter::paint(int month)
{
    if (!device())
    {
        return;
    }

    int width                   = device()->width();
    int height                  = device()->height();
    CalSettings* const settings = CalSettings::instance();
    CalParams& params           = CalSettings::instance()->params;

    // --------------------------------------------------

    // FIXME: magic number 42
    int days[42];
    int startDayOffset = KGlobal::locale()->weekStartDay();

    for (int i = 0; i < 42; ++i)
    {
        days[i] = -1;
    }

    QDate d;
    KGlobal::locale()->calendar()->setDate(d, params.year, month, 1);
    int s = d.dayOfWeek();

    if (s + 7 - startDayOffset >= 7)
    {
        s = s - 7;
    }

    for (int i = s; i < (s + KGlobal::locale()->calendar()->daysInMonth(d)); ++i)
    {
        days[i + (7 - startDayOffset)] = i - s + 1;
    }

    // -----------------------------------------------

    QRect rCal(0, 0, 0, 0);
    QRect rImage(0, 0, 0, 0);
    QRect rCalHeader(0, 0, 0, 0);

    int cellSizeX;
    int cellSizeY;

    switch (params.imgPos)
    {
        case (CalParams::Top):
        {
            rImage.setWidth(width);
            rImage.setHeight((int)(height * params.ratio / (params.ratio + 100)));

            int remainingHeight = height - rImage.height();
            cellSizeX           = (width - 20) / 7;
            cellSizeY           = remainingHeight / 8;

            rCal.setWidth(cellSizeX * 7);
            rCal.setHeight(cellSizeY * 7);

            rCalHeader.setWidth(rCal.width());
            rCalHeader.setHeight(cellSizeY);
            rCalHeader.moveTop(rImage.bottom());
            rCalHeader.moveLeft(width / 2 - rCalHeader.width() / 2);

            rCal.moveTopLeft(rCalHeader.bottomLeft());

            break;
        }

        case (CalParams::Left):
        {
            rImage.setHeight(height);
            rImage.setWidth((int)(width * params.ratio / (params.ratio + 100)));

            int remainingWidth  = width - rImage.width();
            cellSizeX           = (remainingWidth - 20) / 8;
            cellSizeY           = height / 8;

            rCal.setWidth(cellSizeX * 7);
            rCal.setHeight(cellSizeY * 7);

            rCalHeader.setWidth(rCal.width());
            rCalHeader.setHeight(cellSizeY);
            rCalHeader.moveLeft(rImage.right() + cellSizeX);

            rCal.moveTopLeft(rCalHeader.bottomLeft());

            break;
        }

        case (CalParams::Right):
        {
            rImage.setHeight(height);
            rImage.setWidth((int)(width * params.ratio / (params.ratio + 100)));

            int remainingWidth  = width - rImage.width();
            cellSizeX           = (remainingWidth - 20) / 8;
            cellSizeY           = height / 8;

            rCal.setWidth(cellSizeX * 7);
            rCal.setHeight(cellSizeY * 7);

            rCalHeader.setWidth(rCal.width());
            rCalHeader.setHeight(cellSizeY);
            rCal.moveTop(rCalHeader.bottom());

            rImage.moveLeft(width - rImage.width());

            break;
        }

        default:
            return;
    }

    int fontPixels = cellSizeX / 3;
    params.baseFont.setPixelSize(fontPixels);

    // ---------------------------------------------------------------

    fillRect(0, 0, width, height, Qt::white);
    setFont(params.baseFont);

    // ---------------------------------------------------------------

    save();
    QFont f(params.baseFont);
    f.setBold(true);
    f.setPixelSize(f.pixelSize() + 5);
    setFont(f);
    drawText(rCalHeader, Qt::AlignLeft | Qt::AlignVCenter, QString::number(params.year));
    drawText(rCalHeader, Qt::AlignRight | Qt::AlignVCenter,
             KGlobal::locale()->calendar()->monthName(month, params.year));
    restore();

    // ---------------------------------------------------------------

    int   sx, sy;
    QRect r, rsmall, rSpecial;

    r.setWidth(cellSizeX);
    r.setHeight(cellSizeY);

    int index = 0;

    save();

    setPen(Qt::red);
    sy = rCal.top();

    for (int i = 0; i < 7; ++i)
    {
        int dayname = i + startDayOffset;

        if (dayname > 7)
        {
            dayname = dayname - 7;
        }

        sx     = cellSizeX * i + rCal.left();
        r.moveTopLeft(QPoint(sx, sy));
        rsmall = r;
        rsmall.setWidth(r.width() - 2);
        rsmall.setHeight(r.height() - 2);
        drawText(rsmall, Qt::AlignRight | Qt::AlignBottom,
                 KGlobal::locale()->calendar()->weekDayName(dayname, KCalendarSystem::ShortDayName));
    }

    restore();

    for (int j = 0; j < 6; ++j)
    {
        sy = cellSizeY * (j + 1) + rCal.top();

        for (int i = 0; i < 7; ++i)
        {
            sx     = cellSizeX * i + rCal.left();
            r.moveTopLeft(QPoint(sx, sy));
            rsmall = r;
            rsmall.setWidth(r.width() - 2);
            rsmall.setHeight(r.height() - 2);

            if (days[index] != -1)
            {
                if (settings->isSpecial(month, days[index]))
                {
                    save();
                    setPen(settings->getDayColor(month, days[index]));
                    drawText(rsmall, Qt::AlignRight | Qt::AlignBottom,
                             QString::number(days[index]));

                    QString descr = settings->getDayDescr(month, days[index]);
                    kDebug() << "Painting special info: '" << descr
                             << "' for date " << days[index] << "/"
                             << month;
                    rSpecial = rsmall;
                    rSpecial.translate(2, 0);
                    QFont f(params.baseFont);
                    f.setPixelSize(f.pixelSize() / 3);
                    setFont(f);

                    drawText(rSpecial, Qt::AlignLeft | Qt::AlignTop, descr);

                    restore();
                }
                else
                {
                    drawText(rsmall, Qt::AlignRight | Qt::AlignBottom,
                             QString::number(days[index]));
                }
            }

            index++;
        }
    }

    // ---------------------------------------------------------------

    if (params.drawLines)
    {
        sx = rCal.left();

        for (int j = 0; j < 8; ++j)
        {
            sy = cellSizeY * j + rCal.top();
            drawLine(sx, sy, rCal.right(), sy);
        }

        sy = rCal.top();

        for (int i = 0; i < 8; ++i)
        {
            sx = cellSizeX * i + rCal.left();
            drawLine(sx, sy, sx, rCal.bottom());
        }
    }

    // Check if RAW file.
    if (KPMetadata::isRawFile(imagePath_))
    {
        KDcrawIface::KDcraw::loadRawPreview(image_, imagePath_.path());
    }
    else
    {
        image_.load(imagePath_.path());
    }

    if (image_.isNull())
    {
        fillRect(rImage, Qt::blue);
    }
    else
    {
        if ( orientation_ != KPMetadata::ORIENTATION_UNSPECIFIED )
        {
            QMatrix matrix = RotationMatrix::toMatrix(orientation_);
            image_         = image_.transformed( matrix );
        }

        emit signalProgress(0);

        image_ = image_.scaled(rImage.width(), rImage.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        emit signalTotal(image_.height());

        int h = image_.height();
        int x = rImage.bottomLeft().x() + (rImage.width() - image_.width()) / 2;
        int y = (rImage.height() - h) / 2;

        int blockSize = 10;
        int block = 0;

        while (block < h && !cancelled_)
        {
            if (block + blockSize > h)
            {
                blockSize = h - block;
            }

            drawImage(x, y + block, image_, 0, block, image_.width(), blockSize);
            block += blockSize;
            emit signalProgress(block);
        }

        emit signalFinished();
    }
}

}  // NameSpace KIPICalendarPlugin
