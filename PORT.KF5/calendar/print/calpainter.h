/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : painter class to draw calendar.
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

#ifndef CALPAINTER_H
#define CALPAINTER_H

// Qt includes

#include <QImage>
#include <QObject>
#include <QPainter>

// KDE includes

#include <kurl.h>

// Local includes

#include "kpmetadata.h"

class QPaintDevice;

using namespace KIPIPlugins;

namespace KIPICalendarPlugin
{

class CalPainter : public QObject, public QPainter
{
    Q_OBJECT

public:

    explicit CalPainter(QPaintDevice* const pd);
    virtual ~CalPainter();

    void setImage(const KUrl& imagePath, KPMetadata::ImageOrientation orientation);
    void paint(int month);

Q_SIGNALS:

    void signalTotal(int total);
    void signalProgress(int value);
    void signalFinished();

public Q_SLOTS:

    void cancel();

private:

    bool                         cancelled_;

    KPMetadata::ImageOrientation orientation_;

    QImage                       image_;

    KUrl                         imagePath_;
};

}  // NameSpace KIPICalendarPlugin

#endif // CALPAINTER_H
