/* ============================================================
 * File  : monthwidget.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-03
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

#ifndef MONTHWIDGET_H
#define MONTHWIDGET_H

#include <qframe.h>

class QPainter;
class QPixmap;
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class KURL;

#include <libkipi/interface.h>
namespace DKCalendar
{

class MonthWidget : public QFrame
{
    Q_OBJECT

public:

    MonthWidget( KIPI::Interface* interface, QWidget *parent, int month);
    ~MonthWidget();

    KURL imagePath();

protected:

    void drawContents(QPainter *p);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void mouseReleaseEvent(QMouseEvent* e);

private:

    int      month_;
    KURL  imagePath_;
    QPixmap *pixmap_;
    KIPI::Interface* interface_;
private slots:

   void slotGotThumbnaiL(const KURL& url, const QPixmap& pix);
};

}

#endif /* MONTHWIDGET_H */
