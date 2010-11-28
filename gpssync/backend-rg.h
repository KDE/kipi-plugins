/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-05-12
 * @brief  Abstract backend class for reverse geocoding.
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

#ifndef BACKEND_RG_H
#define BACKEND_RG_H

// local includes

#include "kipiimageitem.h"

namespace KIPIGPSSyncPlugin
{

class RGBackend : public QObject
{
    Q_OBJECT

public:

    RGBackend(QObject* const parent);
    RGBackend();
    virtual ~RGBackend();
    
    virtual void callRGBackend(const QList<RGInfo>&, const  QString&) = 0;
    virtual QString getErrorMessage();
    virtual QString backendName();
    virtual void cancelRequests() = 0;

Q_SIGNALS:

    /**
     * @brief Emitted whenever some items are ready
     */
    void signalRGReady(QList<RGInfo>&);

};

} //KIPIGPSPlugin

#endif /* BACKEND_RG_H */
