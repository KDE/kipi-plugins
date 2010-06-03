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

#ifndef SEARCHBACKEND_H
#define SEARCHBACKEND_H

// Qt includes

#include <QObject>

// local includes

#include <worldmapwidget2/worldmapwidget2_primitives.h>
#include "gpsreversegeocodingwidget.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class SearchBackendPrivate;
class SearchBackend : public QObject
{
Q_OBJECT

public:
    class SearchResult
    {
    public:
        SearchResult()
        {
        }

        typedef QList<SearchResult> List;

        WMW2::WMWGeoCoordinate coordinates;
        QString name;
        WMW2::WMWGeoCoordinate::Pair boundingBox;
        QString internalId;
    };

    SearchBackend(QObject* const parent = 0);
    ~SearchBackend();

    bool search(const QString& backendName, const QString& searchTerm);
    SearchResult::List getResults() const;
    QString getErrorMessage() const;
    QList<QPair<QString, QString> >  getBackends() const;

Q_SIGNALS:
    void signalSearchCompleted();

private Q_SLOTS:
    void slotData(KIO::Job* kioJob, const QByteArray& data);
    void slotResult(KJob* kJob);

private:
    SearchBackendPrivate* const d;
};

} /* KIPIGPSSyncPlugin */

#endif /* SEARCHBACKEND_H */
