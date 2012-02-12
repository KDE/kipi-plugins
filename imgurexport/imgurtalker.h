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
#define _IMGUR_API_KEY "2da1cc4923f33dc72885aa32adede5c3";
//#define _IMGUR_API_KEY "2da1cc4923f33dc72885aa32adede5c3-err";

// Qt
#include <QWidget>
#include <QObject>
#include <QFileInfo>
#include <QDateTime>

// KDE
#include <kurl.h>
#include <kio/jobclasses.h>


// kipi
#include <libkipi/interface.h>

namespace KIO
{
    class Job;
}

using namespace KIPI;

namespace KIPIImgurExportPlugin
{

    struct ImgurError
    {
        QString message;
        QString request;
        enum ImgurMethod
        {
            POST = 0,
            GET,
            HEAD
        } method;
        enum ImgurFormat
        {
            XML = 0,
            JSON
        } format;
        QVariant parameters;
    };


    struct ImgurSuccess
    {
        struct ImgurImage
        {
            QString name;
            QString title;
            QString caption;
            QString hash;
            QString deletehash;
            QDateTime datetime;
            QString type; // maybe enum
            bool animated;
            uint width;
            uint height;
            uint size;
            uint views;
            qulonglong bandwidth;
        } image;

        struct ImgurLinks
        {
            KUrl original;
            KUrl imgur_page;
            KUrl delete_page;
            KUrl small_square;
            KUrl large_thumbnail;
        } links;
    };

    class ImgurTalker : public QWidget
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
            IE_LOGIN = 0,
            IE_ADDPHOTO,
            IE_REMOVEPHOTO
        };
    public:
        ImgurTalker (Interface* iface, QWidget* parent = 0);
        ~ImgurTalker();

        const QString getStatusError (ImgurTalker::ServerStatusCode code);

        void startUpload ();

        void cancel ();
//        void dataReq(KIO::Job* job, QByteArray &data);


    Q_SIGNALS:
        void signalUploadStart( const KUrl& url );
        void signalError( const QString& msg );
        void signalBusy( bool val );
        void signalUploadProgress(int);
        void signalUploadDone(const KUrl& url, bool success);

    private:
        QString         m_apiKey;
        KUrl            m_exportUrl;
        KUrl            m_removeUrl;
        QString         m_userAgent;

        QWidget*        m_parent;
        Interface*      m_interface;
        QByteArray      m_buffer;

        State           m_state;
        KUrl            m_currentUrl;

        KIO::Job*       m_job;
//        QMap<KIO::Job*, QByteArray> m_jobData;
        bool imageUpload (KUrl filePath);
        bool imageDelete (QString hash);
        bool parseResponseImageUpload (QByteArray data);

    private Q_SLOTS:
        void slotResult (KJob *job);
        void data (KIO::Job* job, const QByteArray &data);
        void slotAddItems (const KUrl::List& list);

    };
} // namespace KIPIImgurTalkerPlugin
#endif // IMGURTALKER_H
