/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : Backend for reverse geocoding using geonames.org (non-US)
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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

#ifndef BACKEND_GEONAMES_RG_H
#define BACKEND_GEONAMES_RG_H

//kde includes

#include <kmainwindow.h>
#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <klocale.h>

//Qt includes

#include <QWidget>
#include <QList>
#include <QMap>
#include <QString>

//local includes

#include "backend-rg.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class BackendGeonamesRG : public RGBackend
{
    Q_OBJECT

public:

    BackendGeonamesRG(QObject* const parent);
    virtual ~BackendGeonamesRG();
    QMap<QString, QString> makeQMapFromXML(const QString& xmlData);

    virtual void callRGBackend(const QList<RGInfo>& rgList, const QString& language);
    virtual QString getErrorMessage();
    virtual QString backendName();
    virtual void cancelRequests();

private Q_SLOTS:
  
    void nextPhoto(); 
    void dataIsHere(KIO::Job* kJob, const QByteArray &);
    void slotResult(KJob* kJob);

private:

    class BackendGeonamesRGPrivate;
    BackendGeonamesRGPrivate* const d;
};


} //KIPIGISSyncPlugin

#endif /* BACKEND_GEONAMES_RG_H */
