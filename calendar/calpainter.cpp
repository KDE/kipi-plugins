/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-11-03
 * Description : painter class to draw calendar.
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

#include "calpainter.h"
#include "calpainter.moc"

// Qt includes.

#include <QDate>
#include <QFileInfo>
#include <QImage>
#include <QMatrix>
#include <QPaintDevice>
#include <QRect>
#include <QString>
#include <QTimer>

// KDE includes.

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

// LibKDcraw includes.

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes.

#include "calformatter.h"
#include "calsettings.h"

namespace KIPICalendarPlugin
{

CalPainter::CalPainter(QPaintDevice *pd, CalFormatter *formatter)
          : QPainter(pd)
{
    angle_     = 0;
    timer_     = new QTimer( this );
    formatter_ = formatter;
    year_      = KGlobal::locale()->calendar()->year(QDate::currentDate());
    month_     = KGlobal::locale()->calendar()->month(QDate::currentDate());

    connect(timer_, SIGNAL(timeout()),
            this, SLOT(paintNextBlock()));
}

CalPainter::~CalPainter()
{
    delete timer_;
}

void CalPainter::setYearMonth(int year, int month)
{
    year_  = year;
    month_ = month;
}

void CalPainter::setImage(const KUrl &imagePath, int angle)
{
    imagePath_ = imagePath;
    angle_     = angle;
}

void CalPainter::paint(bool isPreview)
{
    if (!device()) return;

    int width  = device()->width();
    int height = device()->height();

    CalParams& params = CalSettings::instance()->params;

    // --------------------------------------------------

    int days[42];
    int startDayOffset = KGlobal::locale()->weekStartDay();

    for (int i=0; i<42; i++)
        days[i] = -1;

    QDate d;
    KGlobal::locale()->calendar()->setYMD(d, year_, month_, 1);
    int s = d.dayOfWeek();

    if (s+7-startDayOffset >= 7)
            s=s-7;

    for (int i=s; i<(s+KGlobal::locale()->calendar()->daysInMonth(d)); i++)
    {
        days[i + (7-startDayOffset)] = i-s+1;
    }

    // -----------------------------------------------

    QRect rCal, rImage, rCalHeader;
    int cellSizeX;
    int cellSizeY;

    rImage     = QRect(0, 0, 0, 0);
    rCal       = QRect(0, 0, 0, 0);
    rCalHeader = QRect(0, 0, 0, 0);

    switch (params.imgPos)
    {
        case(CalParams::Top):
        {
            rImage.setWidth(width);
            rImage.setHeight((int)(height * (params.ratio)/(params.ratio + 100)));

            int remainingHeight = height - rImage.height();
            cellSizeX           = (width - 20)/7;
            cellSizeY           = remainingHeight/8;

            rCal.setWidth(cellSizeX*7);
            rCal.setHeight(cellSizeY*7);

            rCalHeader.setWidth(rCal.width());
            rCalHeader.setHeight(cellSizeY);
            rCalHeader.moveTop(rImage.bottom());
            rCalHeader.moveLeft(width/2 - rCalHeader.width()/2);

            rCal.moveTopLeft(rCalHeader.bottomLeft());

            break;
        }

        case(CalParams::Left):
        {
            rImage.setHeight(height);
            rImage.setWidth((int)(width * (params.ratio)/(params.ratio + 100)));

            int remainingWidth = width - rImage.width();
            cellSizeX          = remainingWidth/8;
            cellSizeY          = (height - 40)/7;

            rCal.setWidth(cellSizeX*7);
            rCal.setHeight(cellSizeY*7);

            rCalHeader.setWidth(rCal.width());
            rCalHeader.setHeight(cellSizeY);
            rCalHeader.moveLeft(rImage.right() + cellSizeX);
            rCalHeader.moveTop(height/2 - (rCalHeader.height()+rCal.height()/2));

            rCal.moveTopLeft(rCalHeader.bottomLeft());

            break;
        }

        case(CalParams::Right):
        {
            rImage.setHeight(height);
            rImage.setWidth((int)(width * (params.ratio)/(params.ratio + 100)));

            int remainingWidth = width - rImage.width();
            cellSizeX          = remainingWidth/8;
            cellSizeY          = (height - 40)/7;

            rCal.setWidth(cellSizeX*7);
            rCal.setHeight(cellSizeY*7);

            rCalHeader.setWidth(rCal.width());
            rCalHeader.setHeight(cellSizeY);
            rCalHeader.moveTop(height/2 - (rCalHeader.height()+rCal.height()/2));
            rCal.moveTop(rCalHeader.bottom());

            rImage.moveLeft(width - rImage.width());

            break;
        }

        default:
            return;
    }

    int fontPixels = cellSizeX/3;
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
    drawText(rCalHeader, Qt::AlignLeft|Qt::AlignVCenter, QString::number(year_));
    drawText(rCalHeader, Qt::AlignRight|Qt::AlignVCenter,
             KGlobal::locale()->calendar()->monthName(month_, year_));
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
    for (int i=0; i<7; i++)
    {
        int dayname = i + startDayOffset;
        if (dayname > 7)
            dayname = dayname-7;

        sx = cellSizeX * i + rCal.left();
        r.moveTopLeft(QPoint(sx,sy));
        rsmall = r;
        rsmall.setWidth(r.width() - 2);
        rsmall.setHeight(r.height() - 2);
        drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                         KGlobal::locale()->calendar()->weekDayName(dayname,
                         KCalendarSystem::ShortDayName));
    }

    restore();

    for (int j=0; j<6; j++)
    {
        sy = cellSizeY * (j + 1) + rCal.top();
        for (int i=0; i<7; i++)
        {
            sx = cellSizeX * i + rCal.left();
            r.moveTopLeft(QPoint(sx,sy));
            rsmall = r;
            rsmall.setWidth(r.width() - 2);
            rsmall.setHeight(r.height() - 2);
            if (days[index] != -1)
            {
                if (!isPreview && formatter_ && formatter_->isSpecial(month_, days[index]))
                {
                    save();
                    setPen( formatter_->getDayColor(month_, days[index]) );
                    drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                             QString::number(days[index]));

                    QString descr = formatter_->getDayDescr(month_, days[index]);
                    kDebug(51000) << "Painting special info: '" << descr
                                  << "' for date " << days[index] << "/"
                                  << month_ << endl;
                    rSpecial = rsmall;
                    rSpecial.translate(2,0);
                    QFont f(params.baseFont);
                    f.setPixelSize(f.pixelSize() / 3);
                    setFont(f);

                    drawText(rSpecial, Qt::AlignLeft|Qt::AlignTop, descr);

                    restore();
                }
                else
                {
                    drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                             QString::number(days[index]));
                }
                drawText(rsmall, Qt::AlignRight|Qt::AlignBottom,
                         QString::number(days[index]));
            }
            index++;
        }
    }

    // ---------------------------------------------------------------

    if (params.drawLines)
    {
        sx = rCal.left();
        for (int j=0; j<8; j++)
        {
            sy = cellSizeY * j + rCal.top();
            drawLine(sx,sy,rCal.right(),sy);
        }

        sy = rCal.top();
        for (int i=0; i<8; i++)
        {
            sx = cellSizeX * i + rCal.left();
            drawLine(sx,sy,sx,rCal.bottom());
        }
    }

    // Check if RAW file.
    QFileInfo fi(imagePath_.path());

#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    if (rawFilesExt.toUpper().contains( fi.suffix().toUpper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(image_, imagePath_.path());
    else
        image_.load(imagePath_.path());

    if (image_.isNull())
    {
        fillRect(rImage, Qt::blue);
    }
    else
    {
        if ( angle_ != 0 )
        {
            QMatrix matrix;
            matrix.rotate( angle_ );
            image_ = image_.transformed( matrix );
        }

        emit signalProgress( 0 );
        image_ = image_.scaled( rImage.width(), rImage.height(), Qt::KeepAspectRatio );

        emit signalTotal( image_.height() );

        x_ = (rImage.width() - image_.width()) / 2;
        y_ = (rImage.height() - image_.height()) / 2;

        block_ = 0;
        timer_->start(10);
    }
}

void CalPainter::paintNextBlock()
{
    const int h = image_.height();
    const int blockSize = qMax(10, block_ - h);

    if (!timer_->isActive())
        return;

    if (block_ >= h)
    {
        timer_->stop();
        emit signalProgress( h );
        emit signalFinished();
    }
    else
    {
        drawImage( x_, y_ + block_, image_, 0, block_, image_.width(), blockSize );
        block_ += blockSize;
        emit signalProgress( block_ );
    }
}

}  // NameSpace KIPICalendarPlugin
