/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description :  calendar widget used for showing a preview
 *                of the active template.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
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

#include "calwidget.moc"

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "calpainter.h"
#include "kpimageinfo.h"
#include "calsettings.h"

using namespace KIPIPlugins;

namespace KIPICalendarPlugin
{

CalWidget::CalWidget(QWidget* const parent)
    : QWidget(parent, 0), _current(1)
{
    setAttribute(Qt::WA_NoBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

CalWidget::~CalWidget()
{
}

void CalWidget::paintEvent(QPaintEvent* e)
{
    int month = _current;
    CalPainter painter(this);

    KUrl imgUrl = CalSettings::instance()->image(month);
    KPImageInfo info(imgUrl);
    KPMetadata::ImageOrientation orientation = info.orientation();
    painter.setImage(imgUrl, orientation);

    painter.paint(month);

    Q_UNUSED(e);
}

void CalWidget::recreate()
{
    CalParams& params = CalSettings::instance()->params;

    setFixedSize(params.width, params.height);
    update();
}

}  // NameSpace KIPICalendarPlugin
