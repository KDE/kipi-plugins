/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-06-01
 * @brief  A simple backend to search OSM and Geonames.org.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
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

#ifndef SEARCHBACKEND_H
#define SEARCHBACKEND_H

// Qt includes

#include <QObject>

// Libkmap includes

#include <libkmap/kmap_primitives.h>

// local includes

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

        KMap::GeoCoordinates coordinates;
        QString name;
        KMap::GeoCoordinates::Pair boundingBox;
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
