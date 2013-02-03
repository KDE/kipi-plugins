/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-06-01
 * @brief  A simple backend to search OSM and Geonames.org.
 *
 * @author Copyright (C) 2010, 2011 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#include "searchbackend.moc"

// Qt includes

#include <QDomDocument>

// KDE includes

#include <kio/job.h>
#include <klocale.h>

// local includes

#include "gpssync_common.h"

namespace KIPIGPSSyncPlugin
{

class SearchBackend::Private
{
public:
    Private()
      : results(),
        kioJob(0),
        runningBackend(),
        searchData(),
        errorMessage()
    {
    }

    SearchBackend::SearchResult::List results;
    KIO::Job*                         kioJob;
    QString                           runningBackend;
    QByteArray                        searchData;
    QString                           errorMessage;
};

SearchBackend::SearchBackend(QObject* const parent)
    : QObject(parent), d(new Private())
{
}

SearchBackend::~SearchBackend()
{
    delete d;
}

bool SearchBackend::search(const QString& backendName, const QString& searchTerm)
{
    d->searchData.clear();
    d->errorMessage.clear();
    d->results.clear();

    if (backendName=="osm")
    {
        d->runningBackend = backendName;

        KUrl jobUrl("http://nominatim.openstreetmap.org/search");
        jobUrl.addQueryItem("format", "xml");
        jobUrl.addQueryItem("q", searchTerm);

        d->kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);
        d->kioJob->addMetaData("User-Agent", getKipiUserAgentName());

        connect(d->kioJob, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(slotData(KIO::Job*,QByteArray)));

        connect(d->kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));

        return true;
    }

    if (backendName=="geonames.org")
    {
        d->runningBackend = backendName;

        // documentation: http://www.geonames.org/export/geonames-search.html

        KUrl jobUrl("http://ws.geonames.org/search");
        jobUrl.addQueryItem("type", "xml");
        jobUrl.addQueryItem("q", searchTerm);

        d->kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);
        d->kioJob->addMetaData("User-Agent", getKipiUserAgentName());

        connect(d->kioJob, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(slotData(KIO::Job*,QByteArray)));

        connect(d->kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));

        return true;
    }

    return false;
}

void SearchBackend::slotData(KIO::Job* kioJob, const QByteArray& data)
{
    Q_UNUSED(kioJob)

    d->searchData.append(data);
}

void SearchBackend::slotResult(KJob* kJob)
{
    if (kJob!=d->kioJob)
    {
        return;
    }

    if (d->kioJob->error())
    {
        d->errorMessage = d->kioJob->errorString();
        emit(signalSearchCompleted());
        return;
    }

    const QString resultString = QString::fromUtf8(d->searchData.constData(), d->searchData.count());

    if (d->runningBackend=="osm")
    {
        QDomDocument doc;
        doc.setContent(resultString); // error-handling
        QDomElement docElement = doc.documentElement(); // error-handling

        for (QDomNode resultNode = docElement.firstChild(); !resultNode.isNull(); resultNode = resultNode.nextSibling())
        {
            QDomElement resultElement = resultNode.toElement();
            if (resultElement.isNull())
            {
                continue;
            }

            if (resultElement.tagName()!="place")
            {
                continue;
            }

            const QString boundingBoxString = resultElement.attribute("boundingbox");
            const QString latString         = resultElement.attribute("lat");
            const QString lonString         = resultElement.attribute("lon");
            const QString displayName       = resultElement.attribute("display_name");
            const QString placeId           = resultElement.attribute("place_id");

            if (latString.isEmpty()||lonString.isEmpty()||displayName.isEmpty())
            {
                continue;
            }

            // now parse the strings:
            qreal lat;
            qreal lon;
            bool okay = false;
            lat       = latString.toDouble(&okay);

            if (okay)
            {
                lon = lonString.toDouble(&okay);
            }

            if (!okay)
            {
                continue;
            }

            SearchResult result;
            result.coordinates = KGeoMap::GeoCoordinates(lat, lon);
            result.name        = displayName;

            if (!placeId.isEmpty())
            {
                result.internalId = "osm-"+placeId;
            }

            // TODO: parse bounding box

            d->results << result;
        }
    }
    else if (d->runningBackend=="geonames.org")
    {
        QDomDocument doc;
        doc.setContent(resultString); // error-handling
        QDomElement docElement = doc.documentElement(); // error-handling
        kDebug()<<docElement.toElement().tagName();

        for (QDomNode resultNode = docElement.firstChild(); !resultNode.isNull(); resultNode = resultNode.nextSibling())
        {
            QDomElement resultElement = resultNode.toElement();
            kDebug()<<resultElement.tagName();

            if (resultElement.isNull())
            {
                continue;
            }

            if (resultElement.tagName()!="geoname")
            {
                continue;
            }

            QString latString;
            QString lonString;
            QString displayName;
            QString geoNameId;

            for (QDomNode resultSubNode = resultElement.firstChild(); !resultSubNode.isNull(); resultSubNode = resultSubNode.nextSibling())
            {
                QDomElement resultSubElement = resultSubNode.toElement();

                if (resultSubElement.isNull())
                {
                    continue;
                }

                if (resultSubElement.tagName()=="lat")
                {
                    latString = resultSubElement.text();
                }
                else if (resultSubElement.tagName()=="lng")
                {
                    lonString = resultSubElement.text();
                }
                else if (resultSubElement.tagName()=="name")
                {
                    displayName = resultSubElement.text();
                }
                else if (resultSubElement.tagName()=="geonameId")
                {
                    geoNameId = resultSubElement.text();
                }
            }

            if (latString.isEmpty()||lonString.isEmpty()||displayName.isEmpty())
            {
                continue;
            }

            // now parse the strings:
            qreal lat;
            qreal lon;
            bool okay = false;
            lat       = latString.toDouble(&okay);

            if (okay)
            {
                lon = lonString.toDouble(&okay);
            }

            if (!okay)
            {
                continue;
            }

            SearchResult result;
            result.coordinates = KGeoMap::GeoCoordinates(lat, lon);
            result.name        = displayName;

            if (!geoNameId.isEmpty())
            {
                result.internalId = "geonames.org-"+geoNameId;
            }

            d->results << result;
        }
    }

    emit(signalSearchCompleted());
}

SearchBackend::SearchResult::List SearchBackend::getResults() const
{
    return d->results;
}

QString SearchBackend::getErrorMessage() const
{
    return d->errorMessage;
}

QList<QPair<QString, QString> > SearchBackend::getBackends() const
{
    QList<QPair<QString, QString> > resultList;
    resultList << QPair<QString, QString>(i18n("GeoNames"), "geonames.org");
    resultList << QPair<QString, QString>(i18n("OSM"), "osm");

    return resultList;
}

} /* KIPIGPSSyncPlugin */
