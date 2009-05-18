/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-28
 * Description : a widget to display a GPS web map locator.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Gerhard Kulzer <gerhard at kulzer dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSMAPWIDGET_H
#define GPSMAPWIDGET_H

// Qt includes

#include <QString>

// KDE includes

#include <khtml_part.h>

namespace KIPIGPSSyncPlugin
{

class GPSMapWidgetPrivate;

class GPSMapWidget : public KHTMLPart
{
    Q_OBJECT

public:

    GPSMapWidget(QWidget* parent);
    ~GPSMapWidget();

    void    setGPSPosition(const QString& lat, const QString& lon);

    void    setZoomLevel(int zoomLevel);
    int     zoomLevel();

    void    setMapType(const QString& mapType);
    QString mapType();

    void    setFileName(const QString& fileName);
    QString fileName();

    void    resized();

Q_SIGNALS:

    void signalNewGPSLocationFromMap(const QString&, const QString&, const QString&);

protected:

    void khtmlMouseMoveEvent(khtml::MouseMoveEvent*);
    void khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *);

private:

    void extractGPSPositionfromStatusbar(const QString& txt);

private:

    GPSMapWidgetPrivate* const d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSMAPWIDGET_H */
