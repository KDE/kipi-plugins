/* ============================================================
 * File  : calwidget.cpp
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

// C++ includes.

#include <iostream>

// Qt includes.
 
#include <qdatetime.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

// Local includes.

#include "calwidget.h"
#include "calpainter.h"
#include "calsettings.h"

namespace KIPICalendarPlugin
{

CalWidget::CalWidget(QWidget *parent)
    : QWidget(parent,0,Qt::WRepaintNoErase|Qt::WResizeNoErase)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    calPainter_ = 0;
    pix_ = new QPixmap();
}

CalWidget::~CalWidget()
{
    delete pix_;
    if (calPainter_)
        delete calPainter_;
}

void CalWidget::paintEvent(QPaintEvent *e)
{
    if (!e || pix_->isNull()) return;
    
    bitBlt(this, 0, 0, pix_, 0, 0, width(), height(), Qt::CopyROP);
}

void CalWidget::recreate()
{
    CalParams& params = CalSettings::instance()->calParams;

    setFixedSize(QSize(params.width,params.height));
    resize(QSize(params.width,params.height));
    pix_->resize(params.width,params.height);

    if (!calPainter_)
        calPainter_ = new CalPainter(pix_);

    calPainter_->setYearMonth(QDate::currentDate().year(),
                              QDate::currentDate().month());
    calPainter_->paint();
    update();    
}

}  // NameSpace KIPICalendarPlugin

