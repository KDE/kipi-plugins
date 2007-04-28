/* ============================================================
 * File  : calwidget.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-01
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

#ifndef CALWIDGET_H
#define CALWIDGET_H

// Qt includes.

#include <qwidget.h>

class QPaintEvent;
class QPixmap;

namespace KIPICalendarPlugin
{

class CalPainter;

class CalWidget : public QWidget
{
public:

    CalWidget(QWidget *parent);
    ~CalWidget();

    void recreate();
    
protected:

    void paintEvent(QPaintEvent *e);
    
private:

    CalPainter *calPainter_;
    QPixmap    *pix_;
    
};

}  // NameSpace KIPICalendarPlugin

#endif /* CALWIDGET_H */
