/* ============================================================
 * File  : calwidget.h
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: calendar widget used for showing a preview
 *              of the active template
 *
 * Copyright 2003 by Renchi Raju
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

#ifndef __CALWIDGET_H__
#define __CALWIDGET_H__

// Qt includes.

#include <QWidget>

class QPaintEvent;
class QPixmap;

namespace KIPICalendarPlugin
{

class CalWidget : public QWidget
{
    Q_OBJECT

public:

    CalWidget(QWidget *parent);
    ~CalWidget();

public slots:

    void recreate();

protected:

    void paintEvent(QPaintEvent *e);

};

}  // NameSpace KIPICalendarPlugin

#endif // __CALWIDGET_H__
