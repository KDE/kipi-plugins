/* ============================================================
 *
 * Date        : 2010-03-26
 * Description : A widget to configure the GPS correlation
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef GPSCORRELATORWIDGET_H
#define GPSCORRELATORWIDGET_H

// KDE includes

#include <kurl.h>

// Qt includes

#include <QWidget>

// WorldMapWidget2 includes

#include <worldmapwidget2/worldmapwidget2_primitives.h>


namespace KIPIGPSSyncPlugin
{

class GPSCorrelatorWidgetPrivate;

class ParsedGPXData
{
public:
    ParsedGPXData()
    : url(),
      isValid(false),
      nPoints(0),
      gpxDataMap()
    {
    }

    KUrl url;
    bool isValid;
    int nPoints;
    QMap<QDateTime, WMW2::WMWGeoCoordinate> gpxDataMap;
};

class GPSCorrelatorWidget : public QWidget
{
Q_OBJECT

public:

    GPSCorrelatorWidget(QWidget* const parent, const int marginHint, const int spacingHint);
    ~GPSCorrelatorWidget();

    void setUIEnabled(const bool state);

protected:

Q_SIGNALS:
    void signalSetUIEnabled(const bool enabledState);

private Q_SLOTS:
    void slotLoadGPXFiles();
    void slotGPXFileReadyAt(int beginIndex, int endIndex);

private:
    GPSCorrelatorWidgetPrivate* const d;
};

} /* KIPIGPSSyncPlugin */

#endif /* GPSCORRELATORWIDGET_H */

