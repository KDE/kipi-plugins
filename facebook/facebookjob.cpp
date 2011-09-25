/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2009 Aleix Pol Gonzalez <aleixpol@kde.org>
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

#include "facebookjob.moc"

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

namespace KIPIFacebookPlugin
{

FacebookJob::FacebookJob(const QString& albumName, const KUrl::List& url, QObject* parent)
    : KJob(parent), m_urls(url), m_talk(0), m_albumName(albumName)
{
    setObjectName("FacebookJob");

    connect(&m_talk, SIGNAL(signalLoginDone(int,QString)),
            this, SLOT(loginDone(int,QString)));

    connect(&m_talk, SIGNAL(signalListAlbumsDone(int,QString,QList<FbAlbum>)),
            this, SLOT(albumList(int,QString,QList<FbAlbum>)));

    connect(&m_talk, SIGNAL(signalCreateAlbumDone(int,QString,QString)),
            this, SLOT(albumCreated(int,QString,QString)));

    connect(&m_talk, SIGNAL(signalAddPhotoDone(int,QString)),
            this, SLOT(addPhoto(int,QString)));
}

void FacebookJob::start()
{
    KConfig cfg(KGlobal::mainComponent());
    KConfigGroup cfgGroup = cfg.group("Facebook");
    QString accessToken   = cfgGroup.readEntry("Access Token", QString());
    uint sessionExpires   = cfgGroup.readEntry("Expires", 0);

    setPercent(20);
    m_talk.authenticate(accessToken, sessionExpires);
}

void FacebookJob::loginDone(int errCode, const QString& error)
{
    if(errCode != 0)
    {
        setError(errCode);
        setErrorText(error);
        emitResult();
        return;
    }

    /*
     * TODO: These config variables never got written for me (Dirk)
     * So I have my doubts that this part of the plugin is actually active.
     */
    KConfig cfg(KGlobal::mainComponent());
    KConfigGroup cfgGroup = cfg.group("Facebook");
    cfgGroup.writeEntry("AccessToken", m_talk.getAccessToken());
    cfgGroup.writeEntry("Expires",     m_talk.getSessionExpires());
    cfgGroup.sync();

    kDebug() << "logged in" << m_talk.getSessionExpires();
    m_talk.listAlbums();
}

void FacebookJob::albumList(int errCode, const QString& errMsg, const QList<FbAlbum>& albums)
{
    if(errCode!=0)
    {
        setError(errCode);
        setErrorText(errMsg);
        emitResult();
        return;
    }

    setPercent(25);
    QString id;

    foreach(const FbAlbum& album, albums)
    {
        if(album.title == m_albumName)
        {
            id = album.id;
            break;
        }
    }

    if(id.isEmpty())
    {
        FbAlbum album;
        album.title       = m_albumName;
        album.description = i18n("Photos taken with KDE");

        m_talk.createAlbum(album);
    }
    else
    {
        m_albumId = id;
        addPhoto(0, QString());
    }

    kDebug() << "listed" << id;
}


void FacebookJob::albumCreated(int errCode, const QString& error, const QString &albumId)
{
    if(errCode != 0)
    {
        setError(errCode);
        setErrorText(error);
        emitResult();
        return;
    }

    setPercent(30);
    m_albumId = albumId;
    addPhoto(0, QString());
    kDebug() << "album created" << albumId;
}

void FacebookJob::addPhoto(int code, const QString& message)
{
    if(code == 0 && !m_urls.isEmpty())
    {
        int count = percent()+(100-percent())/m_urls.size();
        KUrl url  = m_urls.takeLast();
        bool c    = m_talk.addPhoto(url.toLocalFile(), m_albumId, url.fileName());
        Q_ASSERT(c && "could not add the photo to the album");             //FIXME: Report error

        setPercent(count);
    }
    else
    {
        if(code!=0)
        {
            setError(code);
            setErrorText(message);
        }
        emitResult();
    }
}

KIcon FacebookJob::icon() const
{
    return KIcon("facebook");
}

KUrl::List FacebookJob::urls() const
{
    return m_urls;
}

} // namespace KIPIFacebookPlugin
