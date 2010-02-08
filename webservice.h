/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export or import image to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
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
#ifndef WEBSERVICE_H
#define WEBSERVICE_H

// api key from imgur
#define _IMGUR_API_KEY "2da1cc4923f33dc72885aa32adede5c3";

#include "plugin_imgurexport.h"

// Qt
#include <QObject>
#include <QFileInfo>

// KDE
#include <kurl.h>
#include <kio/jobclasses.h>

namespace KIO
{
    class Job;
}

namespace KIPIImgurExportPlugin
{
    class WebService : public QObject
    {
        Q_OBJECT

        enum ServerStatusCode {
            NO_IMAGE = 1000,
            UPLOAD_FAILED,
            TOO_LARGE,
            INVALID_TYPE,
            INVALID_KEY,
            PROCESS_FAILED,
            COPY_FAILED,
            THUMBNAIL_FAILED,
            UPLOAD_LIMIT,
            GIF_TOO_LARGE,
            PNG_TOO_LARGE,
            INVALID_URL,
            URL_UNAVAILABLE,
            INVALID_API_REQUEST = 9000,
            INVALID_RESPONSE_FORMAT
        };
    public:
        WebService (QObject *parent = 0);
        const QString getStatusError (WebService::ServerStatusCode code);
        bool imageUpload (QFileInfo* info);
        bool imageDelete (QString hash);


    Q_SIGNALS:
        void signalError( const QString& msg );
        void signalBusy( bool val );
        void signalAddPhotoDone(int, const QString&);

    private:
        static const QString apiKey;
        QString postUrl;
        KIO::Job*  m_job;

    private Q_SLOTS:
    //    void slotResult (KJob *job);

    };
} // namespace KIPIImgurExportPlugin
#endif // WEBSERVICE_H
