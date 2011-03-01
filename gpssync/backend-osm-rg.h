/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
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


#ifndef BACKEND_OSM_RG_H
#define BACKEND_OSM_RG_H

//kde includes

#include <kurl.h>

//Qt includes

#include <QList>
#include <QMap>

//local includes

#include "backend-rg.h"

namespace KIO { class Job; }
class KJob;

namespace KIPIGPSSyncPlugin
{

class BackendOsmRG : public RGBackend
{
    Q_OBJECT

public:
 
    BackendOsmRG(QObject* const parent);
    virtual ~BackendOsmRG();
    QMap<QString,QString> makeQMapFromXML(const QString& xmlData);

    virtual void callRGBackend(const QList<RGInfo>& rgList,const QString& language);
    virtual QString getErrorMessage();
    virtual QString backendName();
    virtual void cancelRequests();

private Q_SLOTS:

    void nextPhoto(); 
    void dataIsHere(KIO::Job* kJob, const QByteArray &); 
    void slotResult(KJob* kJob);

private:

    class BackendOsmRGPrivate;
    BackendOsmRGPrivate* const d;
};


} // KIPIGISSyncPlugin

#endif /* BACKEND_OSM_RG_H */
