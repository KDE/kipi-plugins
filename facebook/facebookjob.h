/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef FACEBOOK_JOB_H
#define FACEBOOK_JOB_H

// KDE includes

#include <kjob.h>

// Local includes

#include "fbalbum.h"
#include "fbtalker.h"

class KUrl;

namespace KIPIFacebookPlugin
{

class FacebookJob : public KJob
{
Q_OBJECT

public:

    FacebookJob(const QString& albumName, const KUrl::List& url, QObject* parent=0);

    virtual void start();
    virtual QList< KUrl > urls() const;
    virtual KIcon icon() const;

private slots:

    void albumList(int, const QString&, const QList<FbAlbum>&);
    void loginDone(int, const QString&);
    void albumCreated(int, const QString&, const QString &albumId);
    void addPhoto(int code, const QString& message);

private:

    void sendPhoto(const QString &album);

private:

    KUrl::List m_urls;
    FbTalker   talk;
    QString    m_albumName;
    QString    m_albumId;
};

} // namespace KIPIFacebookPlugin

#endif // FACEBOOK_JOB_H
