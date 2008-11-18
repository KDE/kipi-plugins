/* ============================================================
 * File  : monthwidget.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Tom Albers <tomalbers@kde.nl>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: month image selection widget
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2006 by Tom Albers
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

#ifndef __MONTHWIDGET_H__
#define __MONTHWIDGET_H__

// Qt includes.

#include <QSize>
#include <QPixmap>
#include <QPushButton>

// KDE includes.

#include <KUrl>

class QPainter;
class QDropEvent;
class QMouseEvent;
class QPaintEvent;
class QDragEnterEvent;

namespace KIPI
{
    class Interface;
}

namespace KIPICalendarPlugin
{

class MonthWidget : public QPushButton
{
    Q_OBJECT

public:

    MonthWidget( KIPI::Interface* interface, QWidget *parent, int month );
    ~MonthWidget();

    KUrl imagePath();
    void setImage( const KUrl& url );

protected:

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    const QSize thumbSize;
    QPixmap thumb_;
    int     month_;
    KUrl    imagePath_;
    KIPI::Interface* interface_;
    QPixmap thumb() const;
    void setThumb(const QPixmap &pic);
    Q_PROPERTY(QPixmap thumb READ thumb WRITE setThumb);

private slots:

   void gotThumbnail( const KUrl &url, const QPixmap &pix );
};

}  // NameSpace KIPICalendarPlugin

#endif // __MONTHWIDGET_H__
