/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : month image selection widget.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers@kde.nl>
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

#ifndef MONTHWIDGET_H
#define MONTHWIDGET_H

// Qt includes

#include <QPixmap>
#include <QPushButton>
#include <QSize>

// KDE includes

#include <kurl.h>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QPaintEvent;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPICalendarPlugin
{

class MonthWidget : public QPushButton
{
    Q_OBJECT

public:

    MonthWidget(Interface* const interface, QWidget* const parent, int month);
    ~MonthWidget();

    KUrl imagePath() const;
    void setImage(const KUrl& url);
    int month();

Q_SIGNALS:

  void monthSelected(int);

protected:

    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void paintEvent(QPaintEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private Q_SLOTS:

    void gotThumbnail(const KUrl& url, const QPixmap& pix);
    void monthSelected();

private:

    QPixmap thumb() const;
    void setThumb(const QPixmap& pic);
    Q_PROPERTY(QPixmap thumb READ thumb WRITE setThumb)

private:

    const QSize thumbSize;
    QPixmap     thumb_;
    int         month_;
    KUrl        imagePath_;
    Interface*  interface_;
};

}  // NameSpace KIPICalendarPlugin

#endif // MONTHWIDGET_H
