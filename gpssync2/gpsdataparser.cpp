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
#include <klocale.h>

// local includes

#include "gpsdataparser_p.h"

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
    QList<QPair<KUrl, QString> >                loadErrorFiles;
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

void GPSDataParser::loadGPXFiles(const KUrl::List& urls)
{
    d->gpxLoadFutureWatcher = new QFutureWatcher<GPXFileData>(this);

    connect(d->gpxLoadFutureWatcher, SIGNAL(resultsReadyAt(int, int)),
            this, SLOT(slotGPXFilesReadyAt(int, int)));

    d->gpxLoadFuture = QtConcurrent::mapped(urls, GPXFileReader::loadGPXFile);
    d->gpxLoadFutureWatcher->setFuture(d->gpxLoadFuture);

    // results are reported to slotGPXFilesReadyAt
}

void GPSDataParser::slotGPXFilesReadyAt(int beginIndex, int endIndex)
{
    const int nFilesBefore = d->gpxFileDataList.count();

    // note that endIndex is exclusive!
    for (int i=beginIndex; i<endIndex; ++i)
    {
        const GPXFileData nextFile = d->gpxLoadFuture.resultAt(i);
        if (nextFile.isValid)
        {
            d->gpxFileDataList << nextFile;
        }
        else
        {
            d->loadErrorFiles << QPair<KUrl, QString>(nextFile.url, nextFile.loadError);
        }
    }

    // note that endIndex is exclusive!
    emit(signalGPXFilesReadyAt(nFilesBefore, d->gpxFileDataList.count()));

    // are all files done?
    if (d->gpxLoadFuture.isFinished())
    {
        d->gpxLoadFutureWatcher->deleteLater();

        emit(signalAllGPXFilesReady());
    }
}

QDateTime GPXFileReader::ParseTime(QString timeString)
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
 * @brief GPS-correlate items
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
    const bool threadCanceled = d->thread->canceled;
    delete d->thread;
    d->thread = 0;

    if (threadCanceled)
    {
        emit(signalCorrelationCanceled());
    }
    else
    {
        emit(signalAllItemsCorrelated());
    }
}

GPSDataParserThread::GPSDataParserThread(QObject* const parent)
: QThread(parent),
  doCancel(false),
  canceled(false)
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
{sleep(10);
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
        if (doCancel)
        {
            canceled = true;
            return;
        }

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
            if (doCancel)
            {
                canceled = true;
                return;
            }

            const GPSDataParser::GPXFileData& currentFile = fileList.at(f);
            int index = currentIndices.at(f);
            for (; index<currentFile.gpxDataPoints.count(); ++index)
            {
                if (doCancel)
                {
                    canceled = true;
                    return;
                }

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

                correlatedData.nSatellites = dataPoint.nSatellites;
                correlatedData.hDop = dataPoint.hDop;
            }
        }
        else
        {
            // no, we may have to interpolate
            bool canInterpolate = options.interpolate && lastSmallerTime.isValid() && firstBiggerTime.isValid();
            if (canInterpolate)
            {
                canInterpolate = abs(lastSmallerTime.secsTo(itemDateTime)) <= options.interpolationDstTime;
            }
            if (canInterpolate)
            {
                canInterpolate = abs(firstBiggerTime.secsTo(itemDateTime)) <= options.interpolationDstTime;
            }
            if (canInterpolate)
            {
                const GPSDataParser::GPXDataPoint& dataPointBefore = fileList.at(lastIndexPair.first).gpxDataPoints.at(lastIndexPair.second);
                const GPSDataParser::GPXDataPoint& dataPointAfter = fileList.at(firstIndexPair.first).gpxDataPoints.at(firstIndexPair.second);

                const uint tBefore = dataPointBefore.dateTime.toTime_t();
                const uint tAfter = dataPointAfter.dateTime.toTime_t();
                const uint tCor = itemDateTime.toTime_t();
                if (tCor-tBefore!=0)
                {
                    WMW2::WMWGeoCoordinate resultCoordinates;
                    const double latBefore = dataPointBefore.coordinates.lat();
                    const double lonBefore = dataPointBefore.coordinates.lon();
                    const double latAfter = dataPointAfter.coordinates.lat();
                    const double lonAfter = dataPointAfter.coordinates.lon();

                    const qreal interFactor = qreal(tCor-tBefore)/qreal(tAfter-tBefore);
                    resultCoordinates.setLatLon(
                            latBefore + (latAfter - latBefore) * interFactor,
                            lonBefore + (lonAfter - lonBefore) * interFactor
                        );

                    const bool hasAlt = dataPointBefore.coordinates.hasAltitude() && dataPointAfter.coordinates.hasAltitude();
                    if (hasAlt)
                    {
                        const double altBefore = dataPointBefore.coordinates.alt();
                        const double altAfter = dataPointAfter.coordinates.alt();
                        resultCoordinates.setAlt(altBefore + (altAfter - altBefore) * interFactor);
                    }

                    correlatedData.coordinates = resultCoordinates;
                    correlatedData.flags = static_cast<GPSDataParser::GPXFlags>(correlatedData.flags|GPSDataParser::GPXFlagCoordinates);
                }

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

QList<QPair<KUrl, QString> > GPSDataParser::readLoadErrors()
{
    const QList<QPair<KUrl, QString> > result = d->loadErrorFiles;
    d->loadErrorFiles.clear();
    
    return result;
}

void GPSDataParser::cancelCorrelation()
{
    if (d->thread)
    {
        d->thread->doCancel = true;
    }
}

GPXFileReader::GPXFileReader(GPSDataParser::GPXFileData* const dataTarget)
: QXmlDefaultHandler(),
  fileData(dataTarget),
  currentElementPath(),
  currentElements(),
  currentText(),
  currentDataPoint(),
  verifyFoundGPXElement(false)
{

}

/**
 * @brief The parser found characters
 */
bool GPXFileReader::characters(const QString& ch)
{
    currentText+= ch;
    return true;
}

QString GPXFileReader::myQName(const QString& namespaceURI, const QString& localName)
{
    if (namespaceURI=="http://www.topografix.com/GPX/1/0")
        return "gpx:"+localName;

    return namespaceURI+localName;
}

bool GPXFileReader::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
    Q_UNUSED(qName)

    // we always work with the old path
    const QString ePath = currentElementPath;
    const QString eText = currentText;
    const QString eName = myQName(namespaceURI, localName);
    currentElements.removeLast();
    currentText.clear();
    rebuildElementPath();

    if (ePath=="gpx:gpx/gpx:trk/gpx:trkseg/gpx:trkpt")
    {
        if (currentDataPoint.dateTime.isValid()&&currentDataPoint.coordinates.hasCoordinates())
        {
            fileData->gpxDataPoints << currentDataPoint;
        }
        currentDataPoint = GPSDataParser::GPXDataPoint();
    }
    else if (ePath=="gpx:gpx/gpx:trk/gpx:trkseg/gpx:trkpt/gpx:time")
    {
        currentDataPoint.dateTime = ParseTime(eText.trimmed());
    }
    else if (ePath=="gpx:gpx/gpx:trk/gpx:trkseg/gpx:trkpt/gpx:sat")
    {
        bool okay = false;
        int nSatellites = eText.toInt(&okay);
        if (okay&&(nSatellites>=0))
            currentDataPoint.nSatellites = nSatellites;
    }
    else if (ePath=="gpx:gpx/gpx:trk/gpx:trkseg/gpx:trkpt/gpx:hdop")
    {
        bool okay = false;
        qreal hDop = eText.toDouble(&okay);
        if (okay)
            currentDataPoint.hDop = hDop;
    }
    else if (ePath=="gpx:gpx/gpx:trk/gpx:trkseg/gpx:trkpt/gpx:ele")
    {
        bool haveAltitude = false;
        const qreal alt = eText.toDouble(&haveAltitude);
        if (haveAltitude)
        {
            currentDataPoint.coordinates.setAlt(alt);
        }
    }

    return true;
}

bool GPXFileReader::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts)
{
    Q_UNUSED(qName)

    const QString eName = myQName(namespaceURI, localName);
    currentElements << eName;
    rebuildElementPath();
    const QString& ePath = currentElementPath;

    if (ePath=="gpx:gpx/gpx:trk/gpx:trkseg/gpx:trkpt")
    {
        qreal lat;
        qreal lon;
        bool haveLat = false;
        bool haveLon = false;

        for (int i=0; i<atts.count(); ++i)
        {
            const QString attName = myQName(atts.uri(i), atts.localName(i));
            const QString attValue = atts.value(i);
            if (attName=="lat")
            {
                lat = attValue.toDouble(&haveLat);
            }
            else if (attName=="lon")
            {
                lon = attValue.toDouble(&haveLon);
            }
        }

        if (haveLat&&haveLon)
        {
            currentDataPoint.coordinates.setLatLon(lat, lon);
        }
    }
    else if (ePath=="gpx:gpx")
    {
        verifyFoundGPXElement = true;
    }

    return true;
}

void GPXFileReader::rebuildElementPath()
{
    currentElementPath = currentElements.join("/");
}

GPSDataParser::GPXFileData GPXFileReader::loadGPXFile(const KUrl& url)
{
    // TODO: store some kind of error message
    GPSDataParser::GPXFileData parsedData;
    parsedData.url = url;
    parsedData.isValid = false;

    QFile file(url.toLocalFile());
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        parsedData.loadError = i18n("Could not open: %1", file.errorString());
        return parsedData;
    }

    if (file.size()==0)
    {
        parsedData.loadError = i18n("File is empty!");
        return parsedData;
    }

    // TODO: load the file
    GPXFileReader gpxFileReader(&parsedData);
    QXmlSimpleReader reader;
    reader.setContentHandler(&gpxFileReader);
    reader.setErrorHandler(&gpxFileReader);

    QXmlInputSource xmlInputSource(&file);

    // TODO: error handling
    parsedData.isValid = reader.parse(xmlInputSource);
    if (!parsedData.isValid)
    {
        parsedData.loadError = i18n("Parsing error: %1", gpxFileReader.errorString());
        return parsedData;
    }

    parsedData.isValid = !parsedData.gpxDataPoints.isEmpty();
    if (!parsedData.isValid)
    {
        if (!gpxFileReader.verifyFoundGPXElement)
        {
            parsedData.loadError = i18n("No GPX element found - probably not a GPX file!");
        }
        else
        {
            parsedData.loadError = i18n("File is a GPX file, but no datapoints were found!");
        }
        return parsedData;
    }

    // the correlation algorithm relies on sorted data, therefore sort now
    qSort(parsedData.gpxDataPoints.begin(), parsedData.gpxDataPoints.end(), GPSDataParser::GPXDataPoint::EarlierThan);

    return parsedData;
}

} // namespace KIPIGPSSyncPlugin
