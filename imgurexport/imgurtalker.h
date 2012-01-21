/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
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
#ifndef IMGURTALKER_H
#define IMGURTALKER_H

// api key from imgur
#define _IMGUR_API_KEY "2da1cc4923f33dc72885aa32adede5c3asd";

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
    class ImgurTalker : public QObject
    {
        Q_OBJECT

        /**
         * @deprecated
         */
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

        enum State
        {
            FE_LOGIN = 0,
            FE_LISTALBUMS,
            FE_LISTPHOTOS,
            FE_ADDPHOTO,
            FE_UPDATEPHOTO,
            FE_GETPHOTO,
            FE_CHECKTOKEN,
            FE_GETTOKEN,
            FE_CREATEALBUM,
            FE_REMOVEPHOTO
        };
    public:
        ImgurTalker (QObject *parent = 0);
        ~ImgurTalker();

        const QString getStatusError (ImgurTalker::ServerStatusCode code);
        bool imageUpload (QString filePath);
        bool imageDelete (QString hash);
        void cancel ();
//        void dataReq(KIO::Job* job, QByteArray &data);


    Q_SIGNALS:
        void signalError( const QString& msg );
        void signalBusy( bool val );
        void signalUploadDone(int, const QString&);

    private:
        QString         m_apiKey;
        QString         m_exportUrl;
        QString         m_removeUrl;
        QString         m_userAgent;

        QObject*        m_parent;
        QByteArray      m_buffer;

        State           m_state;

        KIO::Job*       m_job;
//        QMap<KIO::Job*, QByteArray> m_jobData;

        void parseResponseImageUpload (QByteArray data);

    private Q_SLOTS:
        void slotResult (KJob *job);
        void data (KIO::Job* job, const QByteArray &data);

    };
} // namespace KIPIImgurTalkerPlugin
#endif // IMGURTALKER_H
