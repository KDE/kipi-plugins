/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-05-12
 * @brief  OSM Nominatim backend for Reverse Geocoding
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2010 by Gabriel Voicu
 *         <a href="mailto:ping dot gabi at gmail dot com">ping dot gabi at gmail dot com</a>
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

#include "backend-osm-rg.moc"

//Qt includes

#include <QDomDocument>
#include <QMap>
#include <QPointer>
#include <QTimer>

//KDE includes

#include <kurl.h>
#include <kdebug.h>
#include <kio/job.h>

//local includes

#include "gpssync_common.h"

namespace KIPIGPSSyncPlugin
{

class OsmInternalJobs
{
public:

    OsmInternalJobs()
      : language(),
        request(),
        data(),
        kioJob(0)
    {
    }

    ~OsmInternalJobs()
    {
        if (kioJob)
            kioJob->deleteLater();
    }

    QString            language;
    QList<RGInfo>      request;
    QByteArray         data;
    QPointer<KIO::Job> kioJob;
};


class BackendOsmRG::Private
{
public:

    Private()
      : jobs(),
        errorMessage()
    {
    }

    QList<OsmInternalJobs> jobs;
    QString                errorMessage;
};

/**
 * @class BackendOsmRG
 *
 * @brief This class calls Open Street Map's reverse geocoding service.
 */

/**
 * Constructor
 * @param Parent object.
 */
BackendOsmRG::BackendOsmRG(QObject* const parent)
    : RGBackend(parent), d(new Private())
{

}

/**
 * Destructor
 */
BackendOsmRG::~BackendOsmRG()
{
    delete d;
}

/**
 * This slot calls Open Street Map's reverse geocoding service for each image.
 */
void BackendOsmRG::nextPhoto()
{
    if (d->jobs.isEmpty())
        return;

    KUrl jobUrl("http://nominatim.openstreetmap.org/reverse");
    jobUrl.addQueryItem("format", "xml");
    jobUrl.addQueryItem("lat", d->jobs.first().request.first().coordinates.latString());
    jobUrl.addQueryItem("lon", d->jobs.first().request.first().coordinates.lonString());
    jobUrl.addQueryItem("zoom", "18");
    jobUrl.addQueryItem("addressdetails", "1");
    jobUrl.addQueryItem("accept-language", d->jobs.first().language);

    d->jobs.first().kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);
    d->jobs.first().kioJob->addMetaData("User-Agent", getKipiUserAgentName());

    connect(d->jobs.first().kioJob, SIGNAL(data(KIO::Job*,QByteArray)), 
            this, SLOT(dataIsHere(KIO::Job*,QByteArray)));
    connect(d->jobs.first().kioJob, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));    
}

/**
 * Takes the coordinate of each image and then connects to Open Street Map's reverse geocoding service.
 * @param rgList A list containing information needed in reverse geocoding process. At this point, it contains only coordinates.
 * @param language The language in which the data will be returned.
 */
void BackendOsmRG::callRGBackend(const QList<RGInfo>& rgList, const QString& language)
{
    d->errorMessage.clear();

    for ( int i = 0; i < rgList.count(); ++i)
    {
        bool foundIt = false;

        for ( int j=0; j < d->jobs.count(); ++j)
        {
            if (d->jobs[j].request.first().coordinates.sameLonLatAs(rgList[i].coordinates))
            {
                d->jobs[j].request << rgList[i];
                d->jobs[j].language = language;
                foundIt             = true;
                break;
             }
        }

        if (!foundIt)
        {
            OsmInternalJobs newJob;
            newJob.request << rgList.at(i);
            newJob.language = language;
            d->jobs<<newJob;
        }
    }

    if (!d->jobs.empty())
        nextPhoto();
}

void BackendOsmRG::dataIsHere(KIO::Job* job, const QByteArray & data)
{
    for (int i = 0; i < d->jobs.count(); ++i)
    {
        if (d->jobs.at(i).kioJob == job)
        {
            d->jobs[i].data.append(data);
            break;
        }
    }
}

/**
 * The data is returned from Open Street Map in a XML. This function translates the XML into a QMap.
 * @param xmlData The returned XML.
 */
QMap<QString,QString> BackendOsmRG::makeQMapFromXML(const QString& xmlData)
{
    QString resultString;
    QMap<QString, QString> mappedData;
    QDomDocument doc;
    doc.setContent(xmlData);

    QDomElement docElem =  doc.documentElement();
    QDomNode n          = docElem.lastChild().firstChild();

    while (!n.isNull())
    {
        QDomElement e = n.toElement();

        if (!e.isNull())
        {
            if ((e.tagName() == QString("country"))         ||
                (e.tagName() == QString("state"))           ||
                (e.tagName() == QString("state_district"))  ||
                (e.tagName() == QString("county"))          ||
                (e.tagName() == QString("city"))            ||
                (e.tagName() == QString("city_district"))   ||
                (e.tagName() == QString("suburb"))          ||
                (e.tagName() == QString("town"))            ||
                (e.tagName() == QString("village"))         ||
                (e.tagName() == QString("hamlet"))          ||
                (e.tagName() == QString("place"))           ||
                (e.tagName() == QString("road"))            ||
                (e.tagName() == QString("house_number")))
            {
                mappedData.insert(e.tagName(), e.text());
                resultString.append(e.tagName() + ':' + e.text() + '\n');
            }
        }

        n = n.nextSibling();
    }

    return mappedData;
}

/**
 * @return Error message, if any.
 */
QString BackendOsmRG::getErrorMessage()
{
    return d->errorMessage;
}

/**
 * @return Backend name.
 */
QString BackendOsmRG::backendName()
{
    return QString("OSM");
}

void BackendOsmRG::slotResult(KJob* kJob)
{
    KIO::Job* const kioJob = qobject_cast<KIO::Job*>(kJob);

    if (kioJob->error())
    {
        d->errorMessage = kioJob->errorString();
        emit(signalRGReady(d->jobs.first().request));
        d->jobs.clear();

        return;
    }

    for (int i = 0; i < d->jobs.count(); ++i)
    {
        if (d->jobs.at(i).kioJob == kioJob)
        {
            QString dataString;
            dataString = QString::fromUtf8(d->jobs[i].data.constData(),qstrlen(d->jobs[i].data.constData()));
            int pos    = dataString.indexOf("<reversegeocode");
            dataString.remove(0,pos);

            QMap<QString, QString> resultMap = makeQMapFromXML(dataString);

            for (int j = 0; j < d->jobs[i].request.count(); ++j)
            {
                d->jobs[i].request[j].rgData = resultMap;
            }

            emit(signalRGReady(d->jobs[i].request));

            d->jobs.removeAt(i);

            if (!d->jobs.empty())
            {
                QTimer::singleShot(500, this, SLOT(nextPhoto()));
            }

            break;
        }
    }
}

void BackendOsmRG::cancelRequests()
{
    d->jobs.clear();
    d->errorMessage.clear();
}

} // KIPIGPSSyncPlugin
