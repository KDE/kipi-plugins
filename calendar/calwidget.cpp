/* ============================================================
 * File  : calwidget.cpp
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

// Local includes.

#include "calwidget.h"
#include "calpainter.h"
#include "calsettings.h"

namespace KIPICalendarPlugin
{

CalWidget::CalWidget(QWidget *parent)
    : QWidget(parent,0)
{
    setAttribute(Qt::WA_NoBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

CalWidget::~CalWidget()
{
}

void CalWidget::paintEvent(QPaintEvent *e)
{
    CalPainter painter( this );
    painter.paint( true );
    Q_UNUSED(e);
}

void CalWidget::recreate()
{
    CalParams& params = CalSettings::instance()->params;

    setFixedSize( params.width, params.height );
    update();
}

}  // NameSpace KIPICalendarPlugin
