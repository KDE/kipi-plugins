/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-19
 * Description : GPS data file parser.
 *               (GPX format http://www.topografix.com/gpx.asp).
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef GPSDATAPARSER_H
#define GPSDATAPARSER_H

// Qt includes

#include <QDateTime>
#include <QThread>

// KDE includes

#include <kurl.h>

// WorldMapWidget2 includes

#include <worldmapwidget2/worldmapwidget2_primitives.h>

class TestGPXParsing;

namespace KIPIGPSSyncPlugin
{

class GPSDataParserPrivate;

class GPSDataParser : public QObject
{
Q_OBJECT
public:

    enum GPXFlags
    {
        GPXFlagCoordinates = 1,
        GPXFlagInterpolated = 2,
        GPXFlagAltitude = 3
    };

    class GPXDataPoint
    {
    public:
        GPXDataPoint()
        {
        }

        QDateTime dateTime;
        WMW2::WMWGeoCoordinate coordinates;

        static bool EarlierThan(const GPXDataPoint& a, const GPXDataPoint& b);

        typedef QList<GPXDataPoint> List;
    };

    class GPXCorrelation
    {
    public:
        GPXCorrelation()
        : dateTime(),
          userData(),
          flags(),
          coordinates()
        {
        }

        typedef QList<GPXCorrelation> List;

        QDateTime dateTime;
        QVariant userData;
        GPXFlags flags;
        WMW2::WMWGeoCoordinate coordinates;
    };

    class GPXCorrelationOptions
    {
    public:
        GPXCorrelationOptions()
        : photoHasSystemTimeZone(false),
          interpolate(false),
          interpolationDstTime(0),
          maxGapTime(0),
          secondsOffset(0)
        {
        }
        bool photoHasSystemTimeZone;
        bool interpolate;
        int interpolationDstTime;
        int maxGapTime;
        int secondsOffset;
    };

    class GPXFileData
    {
    public:

        typedef QList<GPXFileData> List;

        GPXFileData()
        : url(),
          isValid(false),
          nPoints(0),
          gpxDataPoints()
        {
        }

        KUrl url;
        bool isValid;
        int nPoints;
        QList<GPXDataPoint> gpxDataPoints;
    };

    GPSDataParser(QObject* const parent = 0);
    ~GPSDataParser();

    void loadGPXFiles(const KUrl::List& urls);
    void clear();
    const GPXFileData& fileData(const int index) const;
    int fileCount() const;

    void correlate(const GPXCorrelation::List& itemsToCorrelate, const GPXCorrelationOptions& options);

Q_SIGNALS:
    void signalGPXFilesReadyAt(const int startIndex, const int endIndex);
    void signalAllGPXFilesReady();
    void signalItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List& correlatedItems);
    void signalAllItemsCorrelated();

protected:
    static GPXFileData LoadGPXFile(const KUrl& url);
    static QDateTime ParseTime(QString timeString);

private Q_SLOTS:
    void slotGPXFilesReadyAt(int beginIndex, int endIndex);
    void slotThreadItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List& correlatedItems);
    void slotThreadFinished();

private:
    GPSDataParserPrivate* const d;

    friend class ::TestGPXParsing;
};

class GPSDataParserThread : public QThread
{
Q_OBJECT
public:
    GPSDataParserThread(QObject* const parent = 0);
    ~GPSDataParserThread();

    GPSDataParser::GPXCorrelation::List itemsToCorrelate;
    GPSDataParser::GPXCorrelationOptions options;
    GPSDataParser::GPXFileData::List fileList;

protected:
    virtual void run();

Q_SIGNALS:
    void signalItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List& correlatedItems);
};

} // namespace KIPIGPSSyncPlugin

Q_DECLARE_METATYPE(KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List);

#endif  // GPSDATAPARSER_H
