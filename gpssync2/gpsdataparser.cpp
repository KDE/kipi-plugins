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

#include "gpsdataparser.moc"

// Qt includes

#include <qtconcurrentmap.h>
#include <QDomDocument>
#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QString>

// KDE includes

#include <kdebug.h>

namespace KIPIGPSSyncPlugin
{

class GPSDataParserPrivate
{
public:
    GPSDataParserPrivate()
    {
    }

    QFutureWatcher<GPSDataParser::GPXFileData> *gpxLoadFutureWatcher;
    QFuture<GPSDataParser::GPXFileData>         gpxLoadFuture;
    GPSDataParser::GPXFileData::List            gpxFileDataList;
    GPSDataParserThread*                        thread;
};

GPSDataParser::GPSDataParser(QObject* const parent)
: QObject(parent), d(new GPSDataParserPrivate())
{
    qRegisterMetaType<KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List>("KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List");
}

GPSDataParser::~GPSDataParser()
{
    delete d;
}

void GPSDataParser::clear()
{
    d->gpxFileDataList.clear();
}

const GPSDataParser::GPXFileData& GPSDataParser::fileData(const int index) const
{
    return d->gpxFileDataList.at(index);
}

bool GPSDataParser::GPXDataPoint::EarlierThan(const GPXDataPoint& a, const GPXDataPoint& b)
{
    return a.dateTime < b.dateTime;
}

GPSDataParser::GPXFileData GPSDataParser::LoadGPXFile(const KUrl& url)
{
    // TODO: store some kind of error message
    GPXFileData parsedData;
    parsedData.url = url;
    parsedData.isValid = false;

    QFile gpxfile(url.path());

    if (!gpxfile.open(QIODevice::ReadOnly))
        return parsedData;

    QDomDocument gpxDoc("gpx");
    if (!gpxDoc.setContent(&gpxfile))
        return parsedData;

    QDomElement gpxDocElem = gpxDoc.documentElement();
    if (gpxDocElem.tagName()!="gpx")
        return parsedData;

    for (QDomNode nTrk = gpxDocElem.firstChild();
         !nTrk.isNull(); nTrk = nTrk.nextSibling())
    {
        QDomElement trkElem = nTrk.toElement();
        if (trkElem.isNull()) continue;
        if (trkElem.tagName() != "trk") continue;

        for (QDomNode nTrkseg = trkElem.firstChild();
            !nTrkseg.isNull(); nTrkseg = nTrkseg.nextSibling())
        {
            QDomElement trksegElem = nTrkseg.toElement();
            if (trksegElem.isNull()) continue;
            if (trksegElem.tagName() != "trkseg") continue;

            for (QDomNode nTrkpt = trksegElem.firstChild();
                !nTrkpt.isNull(); nTrkpt = nTrkpt.nextSibling())
            {
                QDomElement trkptElem = nTrkpt.toElement();
                if (trkptElem.isNull()) continue;
                if (trkptElem.tagName() != "trkpt") continue;

                GPXDataPoint dataPoint;

                // Get GPS position. If not available continue to next point.
                QString lat = trkptElem.attribute("lat");
                QString lon = trkptElem.attribute("lon");
                if (lat.isEmpty() || lon.isEmpty()) continue;

                dataPoint.coordinates.setLatLon(lat.toDouble(), lon.toDouble());

                // Get metadata of track point (altitude and time stamp)
                for (QDomNode nTrkptMeta = trkptElem.firstChild();
                    !nTrkptMeta.isNull(); nTrkptMeta = nTrkptMeta.nextSibling())
                {
                    QDomElement trkptMetaElem = nTrkptMeta.toElement();
                    if (trkptMetaElem.isNull()) continue;
                    if (trkptMetaElem.tagName() == QString("time"))
                    {
                        // Get GPS point time stamp. If not available continue to next point.
                        const QString time = trkptMetaElem.text();
                        if (time.isEmpty()) continue;
                        dataPoint.dateTime = ParseTime(time);
                    }
                    if (trkptMetaElem.tagName() == QString("ele"))
                    {
                        // Get GPS point altitude. If not available continue to next point.
                        QString ele = trkptMetaElem.text();
                        if (!ele.isEmpty())
                            dataPoint.coordinates.setAlt(ele.toDouble());
                    }
                }

                if (dataPoint.dateTime.isNull())
                    continue;

                parsedData.gpxDataPoints << dataPoint;
                parsedData.nPoints++;

            }
        }
    }

//     for (int i=0; i<60000; ++i)
//     {
//         parsedData.gpxDataMap.insert(QDateTime(), WMW2::WMWGeoCoordinate());
//         parsedData.nPoints++;
//     }

    // the correlation algorithm relies on sorted data, therefore sort now
    qSort(parsedData.gpxDataPoints.begin(), parsedData.gpxDataPoints.end(), GPSDataParser::GPXDataPoint::EarlierThan);

    parsedData.isValid = parsedData.nPoints > 0;
    return parsedData;
}


void GPSDataParser::loadGPXFiles(const KUrl::List& urls)
{
    d->gpxLoadFutureWatcher = new QFutureWatcher<GPXFileData>(this);

    connect(d->gpxLoadFutureWatcher, SIGNAL(resultsReadyAt(int, int)),
            this, SLOT(slotGPXFilesReadyAt(int, int)));

    d->gpxLoadFuture = QtConcurrent::mapped(urls, LoadGPXFile);
    d->gpxLoadFutureWatcher->setFuture(d->gpxLoadFuture);

    // results are reported to slotGPXFilesReadyAt
}

void GPSDataParser::slotGPXFilesReadyAt(int beginIndex, int endIndex)
{
    for (int i=beginIndex; i<endIndex; ++i)
    {
        d->gpxFileDataList << d->gpxLoadFuture.resultAt(i);
    }

    const int nFiles = (endIndex-beginIndex);
    emit(signalGPXFilesReadyAt(d->gpxFileDataList.count()-nFiles, d->gpxFileDataList.count()));

    // are all files done?
    if (d->gpxLoadFuture.progressMaximum() == d->gpxFileDataList.count() )
    {
        d->gpxLoadFutureWatcher->deleteLater();

        emit(signalAllGPXFilesReady());
    }
}

QDateTime GPSDataParser::ParseTime(QString timeString)
{
    if (timeString.isEmpty())
    {
        return QDateTime();
    }

    // we want to be able to parse these formats:
    // "2010-01-14T09:26:02.287-02:00" <-- here we have to cut off the -02:00 and replace it with 'Z'
    // "2010-01-14T09:26:02.287+02:00" <-- here we have to cut off the +02:00 and replace it with 'Z'
    // "2009-03-11T13:39:55.622Z"

    const int timeStringLength = timeString.length();
    const int timeZoneSignPosition = timeStringLength-6;

    // does the string contain a timezone offset?
    int timeZoneOffsetSeconds = 0;
    const int timeZonePlusPosition = timeString.lastIndexOf("+");
    const int timeZoneMinusPosition = timeString.lastIndexOf("-");
    if ( (timeZonePlusPosition == timeZoneSignPosition)||(timeZoneMinusPosition == timeZoneSignPosition) )
    {
        const int timeZoneSign = (timeZonePlusPosition == timeZoneSignPosition) ? +1 : -1;

        // cut off the last digits:
        const QString timeZoneString = timeString.right(6);
        timeString.chop(6);
        timeString+='Z';

        // determine the time zone offset:
        bool okayHour = false;
        bool okayMinute = false;
        const int hourOffset = timeZoneString.mid(1, 2).toInt(&okayHour);
        const int minuteOffset = timeZoneString.mid(4, 2).toInt(&okayMinute);

        if (okayHour&&okayMinute)
        {
            timeZoneOffsetSeconds = hourOffset*3600 + minuteOffset*60;
            timeZoneOffsetSeconds*= timeZoneSign;
        }
    }

    QDateTime theTime = QDateTime::fromString(timeString, Qt::ISODate);
    theTime = theTime.addSecs(-timeZoneOffsetSeconds);

    return theTime;
}

/**
 * @brief GPS-correlated items
 */
void GPSDataParser::correlate(const GPXCorrelation::List& itemsToCorrelate, const GPXCorrelationOptions& options)
{
    d->thread = new GPSDataParserThread(this);
    d->thread->options = options;
    d->thread->fileList = d->gpxFileDataList;
    d->thread->itemsToCorrelate = itemsToCorrelate;

    connect(d->thread, SIGNAL(signalItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List&)),
            this, SLOT(slotThreadItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List&)), Qt::QueuedConnection);

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()), Qt::QueuedConnection);

    d->thread->start();
}

void GPSDataParser::slotThreadItemsCorrelated(const GPXCorrelation::List& correlatedItems)
{
    emit(signalItemsCorrelated(correlatedItems));
}

void GPSDataParser::slotThreadFinished()
{
    delete d->thread;
    d->thread = 0;
    emit(signalAllItemsCorrelated());
}

GPSDataParserThread::GPSDataParserThread(QObject* const parent)
: QThread(parent)
{
}

GPSDataParserThread::~GPSDataParserThread()
{
}

bool GPXCorrelationLessThan(const GPSDataParser::GPXCorrelation& a, const GPSDataParser::GPXCorrelation& b)
{
    return a.dateTime < b.dateTime;
}

void GPSDataParserThread::run()
{
    // sort the items to correlate by time:
    qSort(itemsToCorrelate.begin(), itemsToCorrelate.end(), GPXCorrelationLessThan);

    // now perform the correlation
    // we search all loaded gpx data files in parallel for the points with the best match
    const int nFiles = fileList.count();
    QList<int> currentIndices;
    for (int i=0; i<nFiles; ++i)
        currentIndices << 0;

    for (GPSDataParser::GPXCorrelation::List::iterator it = itemsToCorrelate.begin(); it!=itemsToCorrelate.end(); ++it)
    {
        // GPS device are sync in time by satelite using GMT time.
        QDateTime itemDateTime = it->dateTime.addSecs(options.secondsOffset*(-1));
        if (!options.photosHaveSystemTimeZone)
        {
            // the timezone offset was already included in secondsOffset
            itemDateTime.setTimeSpec(Qt::UTC);
        }
        kDebug()<<itemDateTime;
        // find the last point before our item:
        QDateTime lastSmallerTime;
        QPair<int, int> lastIndexPair;
        QDateTime firstBiggerTime;
        QPair<int, int> firstIndexPair;
        for (int f = 0; f<nFiles; ++f)
        {
            const GPSDataParser::GPXFileData& currentFile = fileList.at(f);
            int index = currentIndices.at(f);
            for (; index<currentFile.gpxDataPoints.count(); ++index)
            {
                const QDateTime& indexTime = currentFile.gpxDataPoints.at(index).dateTime;
                if (indexTime<itemDateTime)
                {
                    bool timeIsBetter = false;
                    if (lastSmallerTime.isValid())
                    {
                        timeIsBetter = (indexTime>lastSmallerTime);
                    }
                    else
                    {
                        timeIsBetter = true;
                    }
                    if (timeIsBetter)
                    {
                        lastSmallerTime = indexTime;
                        lastIndexPair = QPair<int, int>(f, index);
                    }
                }
                else
                {
                    // is it the first time after our item?
                    bool timeIsBetter = false;
                    if (firstBiggerTime.isValid())
                    {
                        timeIsBetter = (indexTime<firstBiggerTime);
                    }
                    else
                    {
                        timeIsBetter = true;
                    }
                    if (timeIsBetter)
                    {
                        firstBiggerTime = indexTime;
                        firstIndexPair = QPair<int, int>(f, index);
                    }

                    break;
                }
            }
            currentIndices[f] = index;
        }

        GPSDataParser::GPXCorrelation correlatedData = *it;

        // do we have a timestamp within maxGap?
        bool canUseTimeBefore = lastSmallerTime.isValid();
        int dtimeBefore = 0;
        if (canUseTimeBefore)
        {
            dtimeBefore = abs(lastSmallerTime.secsTo(itemDateTime));
            canUseTimeBefore = dtimeBefore <= options.maxGapTime;
        }
        bool canUseTimeAfter = firstBiggerTime.isValid();
        int dtimeAfter = 0;
        if (canUseTimeAfter)
        {
            dtimeAfter = abs(firstBiggerTime.secsTo(itemDateTime));
            canUseTimeAfter = dtimeAfter <= options.maxGapTime;
        }
        if (canUseTimeAfter||canUseTimeBefore)
        {
            QPair<int, int> indexToUse(-1, -1);
            if (canUseTimeAfter&&canUseTimeBefore)
            {
                indexToUse = (dtimeBefore < dtimeAfter) ? lastIndexPair:firstIndexPair;
            }
            else if (canUseTimeAfter)
            {
                indexToUse = firstIndexPair;
            }
            else if (canUseTimeBefore)
            {
                indexToUse = lastIndexPair;
            }

            if (indexToUse.first>=0)
            {
                const GPSDataParser::GPXDataPoint& dataPoint = fileList.at(indexToUse.first).gpxDataPoints.at(indexToUse.second);
                correlatedData.coordinates = dataPoint.coordinates;
                correlatedData.flags = static_cast<GPSDataParser::GPXFlags>(correlatedData.flags|GPSDataParser::GPXFlagCoordinates);
            }
        }
        else
        {
            // no, we may have to interpolate
            bool canInterpolate = options.interpolate && lastSmallerTime.isValid() && firstBiggerTime.isValid();
            if (canInterpolate)
            {
                canInterpolate = abs(lastSmallerTime.secsTo(itemDateTime)) < options.interpolationDstTime;
            }
            if (canInterpolate)
            {
                canInterpolate = abs(firstBiggerTime.secsTo(itemDateTime)) < options.interpolationDstTime;
            }
            if (canInterpolate)
            {
                // TODO: we shall interpolate and have items before and after
            }
        }

        if (correlatedData.flags&GPSDataParser::GPXFlagCoordinates)
        {
            GPSDataParser::GPXCorrelation::List readyItems;
            readyItems << correlatedData;
            kDebug()<<"correlated!";
            emit(signalItemsCorrelated(readyItems));
        }
    }
}

int GPSDataParser::fileCount() const
{
    return d->gpxFileDataList.count();
}

} // namespace KIPIGPSSyncPlugin
