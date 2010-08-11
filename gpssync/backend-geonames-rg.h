/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : OSM Nominatim backend for Reverse Geocoding (non-US)
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

class BackendGeonamesRGPrivate;

/** 
 * @class BackendGeonamesRG
 *
 * @brief This class calls Geonames's reverse geocoding service.
 */

class BackendGeonamesRG : public RGBackend
{

    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent object.
     */ 
    BackendGeonamesRG(QObject* const parent);

    /**
     * Destructor
     */
    virtual ~BackendGeonamesRG();

    /**
     * The data is returned from Open Street Map in a XML. This function translates the XML into a QMap.
     * @param xmlData The returned XML.
     */
    QMap<QString, QString> makeQMapFromXML(const QString& xmlData);
    
    /**
     * Takes coordinates from each image and then connects to Open Street Map's reverse geocoding service.
     * @param rgList A list containing information needed in reverse geocoding process. At this point, it contains only coordinates.
     * @param language The language in which the data will be returned.
     */
    virtual void callRGBackend(const QList<RGInfo>& rgList, const QString& language);

    /**
     * @return Error message, if any.
     */ 
    virtual QString getErrorMessage();

    /**
     * @return Backend name.
     */ 
    virtual QString backendName();

private Q_SLOTS:

    /*
     * This function calls Geonames's reverse geocoding service for each image.
     */
    void nextPhoto(); 
    void dataIsHere(KIO::Job*, const QByteArray &); 
    void slotResult(KJob*);
private:
    BackendGeonamesRGPrivate *d;

};


} //KIPIGISSyncPlugin

#endif
