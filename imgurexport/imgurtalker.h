/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

// API key from imgur
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

// LibKIPI includes

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

    QString  message;
    QString  request;
    QVariant parameters;
};

// -----------------------------------------------------------------------------

struct ImgurUploadData
{
    QString title;
    QString caption;
    KUrl    fileUrl;
};

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

    ImgurTalker(Interface* const iface, QWidget* const parent = 0);
    ~ImgurTalker();

//    void startUpload ();
    void        cancel();
    bool        imageUpload(const KUrl& filePath);
    KUrl::List* imageQueue() const;

Q_SIGNALS:

    void signalUploadStart(const KUrl& url);
    void signalUploadProgress(int);
    void signalUploadDone();
    void signalBusy(bool busy);
    void signalError(ImgurError err);
    void signalSuccess(ImgurSuccess success);

private:

    bool imageDelete(const QString& hash);
    bool parseResponseImageUpload(const QByteArray& data);

    bool imageRemove(const QString& hash);
    bool parseResponseImageRemove(const QByteArray& data);

private Q_SLOTS:

    void slotResult(KJob* job);
    void slotData(KIO::Job* job, const QByteArray& data);
    void slotAddItems(const KUrl::List& list);

private:

    class ImgurTalkerPriv;
    ImgurTalkerPriv* const d;
};

} // namespace KIPIImgurTalkerPlugin

#endif // IMGURTALKER_H
