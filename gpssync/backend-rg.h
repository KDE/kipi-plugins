/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : Backend abstract class for reverse geocoding
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

#ifndef BACKEND_RG_H
#define BACKEND_RG_H

//Qt includes

#include <QWidget>

//KDE includes

#include <kurl.h>

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
    
    virtual void callRGBackend(const QList<RGInfo>&,const  QString&) = 0;
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
