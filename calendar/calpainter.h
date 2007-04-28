/* ============================================================
 * File  : calpainter.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-02
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

#ifndef CALPAINTER_H
#define CALPAINTER_H

// Qt includes.

#include <qobject.h>
#include <qvaluelist.h>

// KDE includes.

#include <kurl.h>

class QPaintDevice;
class QPainter;
class QString;
class QTimer;
class QImage;

namespace KIPICalendarPlugin
{

class CalPainter
{
public:

    CalPainter(QPaintDevice *pd);
    ~CalPainter();

    void setYearMonth(int year, int month);
    void paint(bool useDeviceMetrics=false);

private:

    QPaintDevice *pd_;
    int           year_;
    int           month_;
};

void paintCalendar(int year, int month, const QString& imagePath,
                   QPainter *painter,
                   bool useDeviceMetrics=false);

class CalBlockPainter : public QObject
{
    Q_OBJECT

public:

    CalBlockPainter(QObject *parent, int year, int month,
                    const KURL& imagePath, int angle, QPainter *painter);
    ~CalBlockPainter();

signals:

    void signalCompleted();
    void signalProgress(int progress, int total);

private:

    struct Block {
        int x, y;
        int sx, sy;
        int w, h;
    };

    struct Block      *blocks_;
    int                numBlocks_;
    int                currBlock_;
    QTimer            *timer_;
    QPainter          *painter_;
    QImage            *image_;

private slots:

    void slotPaintNextBlock();

};

}  // NameSpace KIPICalendarPlugin

#endif /* CALPAINTER_H */

