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

#ifndef __CALPAINTER_H__
#define __CALPAINTER_H__

// Qt includes.

#include <QImage>
#include <QObject>
#include <QPainter>

// KDE includes.

#include <KUrl>

class QTimer;
class QString;
class QPaintDevice;

namespace KIPICalendarPlugin
{

class CalFormatter;

class CalPainter : public QObject, public QPainter
{
    Q_OBJECT

public:

    CalPainter(QPaintDevice *pd, CalFormatter *formatter = 0);
    virtual ~CalPainter();

    void setYearMonth(int year, int month);
    void setImage(const KUrl &imagePath, int angle);
    void paint(bool isPreview = false);

signals:

    void signalTotal(int total);
    void signalProgress(int value);
    void signalFinished();

public slots:

    void paintNextBlock();

private:

    QImage        image_;
    int           block_, x_, y_;
    int           year_;
    int           month_;
    KUrl          imagePath_;
    int           angle_;
    CalFormatter *formatter_;
    QTimer       *timer_;
};

}  // NameSpace KIPICalendarPlugin

#endif // __CALPAINTER_H__

