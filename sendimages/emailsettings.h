/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-07
 * Description : e-mail settings container.
 *
 * Copyright (C) 2007-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef EMAILSETTING_H
#define EMAILSETTING_H

// Qt includes

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>

namespace KIPISendimagesPlugin
{

class EmailItem
{

public:

    int         rating;         // Image Rating from Kipi host.

    QString     comments;       // Image comments from Kipi host.

    QStringList tags;           // List of keywords from Kipi host.

    QUrl        orgUrl;         // Url of original image.
    QUrl        emailUrl;       // Url of attached image in e-mail (can be resized).
};

// -----------------------------------------------------------------------------------

class EmailSettings
{

public:

    enum EmailClient
    {
        DEFAULT = 0,            // Default e-mail settings from desktop contro; panel.
        BALSA,
        CLAWSMAIL,
        EVOLUTION,
        KMAIL,
        NETSCAPE,
        SYLPHEED,
        SYLPHEEDCLAWS,
        THUNDERBIRD
    };

    enum ImageSize
    {
        VERYSMALL = 0,
        SMALL,
        MEDIUM,
        BIG,
        VERYBIG,
        LARGE,
        FULLHD,
        ULTRAHD
    };

    enum ImageFormat
    {
        JPEG = 0,
        PNG
    };

public:

    EmailSettings()
    {
        addCommentsAndTags      = false;
        imagesChangeProp        = false;
        attachmentLimitInMbytes = 17;
        imageCompression        = 75;
        emailProgram            = DEFAULT;
        imageSize               = MEDIUM;
        imageFormat             = JPEG;
    };

    ~EmailSettings()
    {
    };

    int size() const
    {
        if(imageSize == SMALL)
            return 640;
        else if(imageSize == MEDIUM)
            return 800;
        else if(imageSize == BIG)
            return 1024;
        else if(imageSize == VERYBIG)
            return 1280;
        else if(imageSize == LARGE)
            return 1600;
        else if(imageSize == FULLHD)
            return 1920;
        else if(imageSize == ULTRAHD)
            return 3840;
        else
            return 320; // VERYSMALL
    };

    QString format() const
    {
        if (imageFormat == JPEG)
            return QLatin1String("JPEG");

        return QLatin1String("PNG");
    };

    void setEmailUrl(const QUrl& orgUrl, const QUrl& emailUrl)
    {
        for (QList<EmailItem>::iterator it = itemsList.begin(); it != itemsList.end(); ++it)
        {
            if ((*it).orgUrl == orgUrl)
            {
                (*it).emailUrl = emailUrl;
                return;
            }
        }
    };

    QUrl emailUrl(const QUrl& orgUrl) const
    {
        for (QList<EmailItem>::const_iterator it = itemsList.begin(); it != itemsList.end(); ++it)
        {
            if ((*it).orgUrl == orgUrl)
            {
                return (*it).emailUrl;
            }
        }

        return QUrl();
    };

    qint64 attachementLimitInBytes() const
    {
        qint64 val = attachmentLimitInMbytes * 1024 * 1024;
        return val;
    };

public:

    bool             addCommentsAndTags;
    bool             imagesChangeProp;

    int              imageCompression;

    qint64           attachmentLimitInMbytes;

    QString          tempPath;

    EmailClient      emailProgram;

    ImageSize        imageSize;

    ImageFormat      imageFormat;

    QList<EmailItem> itemsList;
};

}  // namespace KIPISendimagesPlugin

#endif  // EMAILSETTING_H
