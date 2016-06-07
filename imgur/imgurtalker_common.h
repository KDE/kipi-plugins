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

#ifndef IMGURTALKER_COMMON_H
#define IMGURTALKER_COMMON_H

// API key from imgur
#define _IMGUR_API_ANONYMOUS_CLIENT_ID "048d0b40574528c";

#ifdef OAUTH_ENABLED
#   define _IMGUR_API_CONSUMER_KEY "9ad9d48124b7137d9cc9e0397219f52004fcba12a";
#   define _IMGUR_API_CONSUMER_SECRET "26b83f19f6d045006d769869b9c623e0";
#endif //OAUTH_ENABLED

// Qt includes

#include <QDateTime>
#include <QString>
#include <QUrl>

namespace KIPIImgurPlugin
{

class ImgurConnection
{
public:

    static QString APIuploadURL()
    {
        return QLatin1String("https://api.imgur.com/3/image.json");
    }

    static QString APIdeleteURL()
    {
        return QLatin1String("https://api.imgur.com/3/delete.json");
    }

    static QString OAuthTokenEndPoint()
    {
        return QLatin1String("https://api.imgur.com/oauth/request_token");
    }

    static QString OAuthAccessEndPoint()
    {
        return QLatin1String("https://api.imgur.com/oauth/access_token");
    }

    static QString OAuthAuthorizationEndPoint()
    {
        return QLatin1String("https://api.imgur.com/oauth/authorize");
    }

    static QUrl originalURL(QString imageHash)
    {
        return QUrl(QLatin1String("http://i.imgur.com/") + imageHash + QLatin1String(".jpg"));
    } // this is wrong

    static QUrl pageURL(QString imageId)
    {
        return QUrl(QLatin1String("http://imgur.com/") + imageId);
    }

    static QUrl deleteURL(QString deleteHash)
    {
        return QUrl(QLatin1String("http://imgur.com/delete/") + deleteHash);
    }
};

/*
QString ImgurConnection::uploadURL = QLatin1String("https://api.imgur.com/2/upload.json");
QString ImgurConnection::deleteURL = QLatin1String("https://api.imgur.com/2/delete.json");
QString ImgurConnection::OAuthTokenEndPoint = QLatin1String("https://api.imgur.com/oauth/request_token");
QString ImgurConnection::OAuthAccessEndPoint = QLatin1String("https://api.imgur.com/oauth/access_token");
QString ImgurConnection::OAuthAuthorizationEndPoint = QLatin1String("https://api.imgur.com/oauth/authorize");
*/

struct ImgurError
{
    enum ImgurMethod
    {
        POST = 0,
        GET,
        HEAD
    };

    enum ImgurFormat
    {
        XML = 0,
        JSON
    };

    ImgurError()
    {
        method = POST;
        format = XML;
    }

    ImgurMethod method;
    ImgurFormat format;
    QString     message;
    QString     request;
    QVariant    parameters;
};

// -----------------------------------------------------------------------------

struct ImgurUploadData
{
    QString title;
    QString caption;
    QUrl    fileUrl;
};

// -----------------------------------------------------------------------------

struct ImgurSuccess
{
    struct ImgurImage
    {
        QString    id;
        QString    type;       // maybe enum
        QString    name;
        QString    title;
        QString    description;
        QString    deletehash;
        QDateTime  datetime;
        bool       animated;
        uint       width;
        uint       height;
        uint       size;
        uint       views;
        qulonglong bandwidth;
        QUrl       link;
    } image;
};

} // namespace KIPIImgurPlugin

#endif // IMGURTALKER_COMMON_H
