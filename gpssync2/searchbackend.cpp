/* ============================================================
 *
 * Date        : 2010-06-01
 * Description : A simple backend to search OSM and Geonames.org
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

#include "searchbackend.moc"


// Qt includes

#include <QDomDocument>

// KDE includes

#include <kio/job.h>

namespace KIPIGPSSyncPlugin
{

class SearchBackendPrivate
{
public:
    SearchBackendPrivate()
    {
    }

    SearchBackend::SearchResult::List results;
    KIO::Job* kioJob;
    QString runningBackend;
    QByteArray searchData;
    QString errorMessage;
};

SearchBackend::SearchBackend(QObject* const parent)
: QObject(parent), d(new SearchBackendPrivate())
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

        connect(d->kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)),
                this, SLOT(slotData(KIO::Job*, const QByteArray&)));

        connect(d->kioJob, SIGNAL(result(KJob*)),
                this, SLOT(slotResult(KJob*)));

        return true;
    }

    return false;
}

void SearchBackend::slotData(KIO::Job* kioJob, const QByteArray& data)
{
    d->searchData.append(data);
}

void SearchBackend::slotResult(KJob* kJob)
{
    if (kJob!=d->kioJob)
        return;

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
            if (resultElement.isNull()) continue;
            if (resultElement.tagName()!="place") continue;

            const QString boundingBoxString = resultElement.attribute("boundingbox");
            const QString latString = resultElement.attribute("lat");
            const QString lonString = resultElement.attribute("lon");
            const QString displayName = resultElement.attribute("display_name");

            if (latString.isEmpty()||lonString.isEmpty()||displayName.isEmpty())
                continue;

            // now parse the strings:
            qreal lat;
            qreal lon;
            bool okay = false;
            lat = latString.toDouble(&okay);
            if (okay)
                lon = lonString.toDouble(&okay);

            if (!okay)
                continue;

            SearchResult result;
            result.coordinates = WMW2::WMWGeoCoordinate(lat, lon);
            result.name = displayName;

            // TODO: parse bounding box

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

} /* KIPIGPSSyncPlugin */
