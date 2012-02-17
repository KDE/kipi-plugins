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

// Qt includes

#include <QWidget>
#include <QObject>
#include <QFileInfo>
#include <QDateTime>

// KDE includes

#include <kurl.h>
#include <kio/jobclasses.h>

// LibKipi includes


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

// -----------------------------------------------------------------------------

struct ImgurSuccess
{
    struct ImgurImage
    {
        QString    name;
        QString    title;
        QString    caption;
        QString    hash;
        QString    deletehash;
        QDateTime  datetime;
        QString    type; // maybe enum
        bool       animated;
        uint       width;
        uint       height;
        uint       size;
        uint       views;
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

// ----------------------------------------------------------------------

class ImgurTalker : public QWidget
{
    Q_OBJECT

    enum State
    {
        IE_LOGIN = 1,
        IE_ADDPHOTO,
        IE_REMOVEPHOTO
    };

public:

    ImgurTalker (Interface* iface, QWidget* parent = 0);
    ~ImgurTalker();

    void startUpload ();
    void cancel ();
    KUrl::List*      processQueue();

Q_SIGNALS:
    void signalBusy( bool busy);
    void signalUploadStart( const KUrl& url );
    void signalUploadProgress(int);
    void signalUploadDone(const KUrl& url, bool success);
    void signalError( const QString& msg );

private:

    bool imageUpload (KUrl filePath);
    bool imageDelete (QString hash);
    bool parseResponseImageUpload (QByteArray data);

    QString         m_apiKey;
    QString         m_userAgent;

    QWidget*        m_parent;
    Interface*      m_interface;
    QByteArray      m_buffer;

    State           m_state;
    KUrl            m_currentUrl;
    KUrl::List      m_processQueue;
    KIO::Job*       m_job;

    bool imageRemove (QString hash);
    bool parseResponseImageRemove (QByteArray data);

private Q_SLOTS:

    void slotResult (KJob *job);
    void data (KIO::Job* job, const QByteArray &data);
    void slotAddItems (const KUrl::List& list);
};

} // namespace KIPIImgurTalkerPlugin

#endif // IMGURTALKER_H
