/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-07
 * Description : e-mail settings container.
 *
 * Copyright (C) 2007-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QList>
#include <QString>
#include <QStringList>

// KDE includes

#include <kurl.h>

namespace KIPISendimagesPlugin
{

class EmailItem
{

public:

    int         rating;         // Image Rating from Kipi host.

    QString     comments;       // Image comments from Kipi host.

    QStringList tags;           // List of keywords from Kipi host.

    KUrl        orgUrl;         // Url of original image.
    KUrl        emailUrl;       // Url of attached image in e-mail (can be resized).
};

// -----------------------------------------------------------------------------------

class EmailSettings
{

public:

    enum EmailClient
    {
        DEFAULT = 0,        // Default e-mail settings from KDE control panel.
        BALSA,
        CLAWSMAIL,
        EVOLUTION,
        GMAILAGENT,
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
        LARGE
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
        emailProgram            = KMAIL;
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
        else
            return 320; // VERYSMALL
    };

    QString format() const
    {
        if (imageFormat == JPEG)
            return QString("JPEG");

        return QString("PNG");
    };

    void setEmailUrl(const KUrl& orgUrl, const KUrl& emailUrl)
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

    KUrl emailUrl(const KUrl& orgUrl) const
    {
        for (QList<EmailItem>::const_iterator it = itemsList.begin(); it != itemsList.end(); ++it)
        {
            if ((*it).orgUrl == orgUrl)
            {
                return (*it).emailUrl;
            }
        }
        return KUrl();
    };

    qint64 attachementLimitInBytes() const
    {
        qint64 val = attachmentLimitInMbytes * 1024 * 1024;
        return val;
    }

public:

    bool             addCommentsAndTags;
    bool             imagesChangeProp;

    int              imageCompression;

    int              attachmentLimitInMbytes;

    QString          tempPath;
    QString          tempFolderName;

    EmailClient      emailProgram;

    ImageSize        imageSize;

    ImageFormat      imageFormat;

    QList<EmailItem> itemsList;
};

}  // namespace KIPISendimagesPlugin

#endif  // EMAILSETTING_H
