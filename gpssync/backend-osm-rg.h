/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : OSM Nominatim backend for Reverse Geocoding
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
#ifndef BACKEND_OSM_RG_H
#define BACKEND_OSM_RG_H

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

//local includes
#include "backend-rg.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class BackendOsmRGPrivate;

class BackendOsmRG : public RGBackend
{

    Q_OBJECT

public:

    BackendOsmRG(QObject* const parent);
    virtual ~BackendOsmRG();
    QMap<QString,QString> makeQMapFromXML(QString);

    virtual void callRGBackend(QList <RGInfo>, QString);
    virtual QString getErrorMessage();
    virtual QString backendName();

private Q_SLOTS:

    void nextPhoto(); 
    void dataIsHere(KIO::Job*, const QByteArray &); 
    void slotResult(KJob*);

private:
    BackendOsmRGPrivate *d;

};


} //KIPIGISSyncPlugin

#endif
