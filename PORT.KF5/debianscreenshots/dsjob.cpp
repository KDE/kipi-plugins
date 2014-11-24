/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#include "dsjob.moc"

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

namespace KIPIDebianScreenshotsPlugin
{

DebianScreenshotsJob::DebianScreenshotsJob(const QString& packageName, const KUrl::List& url, QObject* const parent)
    : KJob(parent), m_urls(url), m_packageName(packageName)
{
    setObjectName("DebianScreenshotsJob");
/*
    connect(&talk, SIGNAL(signalLoginDone(int,QString)),
            this, SLOT(loginDone(int,QString)));

    connect(&talk, SIGNAL(signalListAlbumsDone(int,QString,QList<FbAlbum>)),
            this, SLOT(albumList(int,QString,QList<FbAlbum>)));

    connect(&talk, SIGNAL(signalCreateAlbumDone(int,QString,QString)),
            this, SLOT(albumCreated(int,QString,QString)));

    connect(&talk, SIGNAL(signalAddPhotoDone(int,QString)),
            this, SLOT(addPhoto(int,QString)));*/
}

void DebianScreenshotsJob::start()
{
//     KConfig cfg(KGlobal::mainComponent());
//     KConfigGroup cfgGroup = cfg.group("DebianScreenshots");
//     QString sessionKey    = cfgGroup.readEntry("Key", QString());
//     QString sessionSecret = cfgGroup.readEntry("Secret", QString());
//     uint sessionExpires   = cfgGroup.readEntry("Expires", 0);
// 
//     setPercent(20);
//     talk.authenticate(sessionKey, sessionSecret, sessionExpires);
}

//void DebianScreenshotsJob::loginDone(int errCode, const QString& error)
//{
//     if(errCode != 0)
//     {
//         setError(errCode);
//         setErrorText(error);
//         emitResult();
//         return;
//     }
//     
//     KConfig cfg(KGlobal::mainComponent());
//     KConfigGroup cfgGroup = cfg.group("DebianScreenshots");
//     cfgGroup.writeEntry("Key",     talk.getSessionKey());
//     cfgGroup.writeEntry("Secret",  talk.getSessionSecret());
//     cfgGroup.writeEntry("Expires", talk.getSessionExpires());
//     cfgGroup.sync();
//     
//     kDebug() << "logged in" << talk.getSessionExpires();
//     talk.listAlbums();
//}

//void DebianScreenshotsJob::packageList(int errCode, const QString& errMsg, const QList<DsPackage>& packages)
//{
//     if(errCode!=0)
//     {
//         setError(errCode);
//         setErrorText(errMsg);
//         emitResult();
//         return;
//     }
//     
//     setPercent(25);
//     QString id = QString();
//     foreach(const FbAlbum& album, albums)
//     {
//         if(album.title==m_albumName)
//         {
//             id = album.id;
//             break;
//         }
//     }
//     
//     if(id.isEmpty())
//     {
//         FbAlbum album;
//         album.title=m_albumName;
//         album.description=i18n("Photos taken with KDE");
//         
//         talk.createAlbum(album);
//     }
//     else
//     {
//         m_albumId = id;
//         addPhoto(0, QString());
//     }
//     
//     kDebug() << "listed" << id;
//}

void DebianScreenshotsJob::addScreenshot(int code, const QString& message)
{
//     if(code==0 && !m_urls.isEmpty()) {
//         int count = percent()+(100-percent())/m_urls.size();
//         KUrl url = m_urls.takeLast();
//         bool c = talk.addPhoto(url.toLocalFile(), m_albumId, url.fileName());
//         Q_ASSERT(c && "could not add the photo to the album");             //FIXME: Report error
//         
//         setPercent(count);
//     } else {
//         if(code!=0) {
//             setError(code);
//             setErrorText(message);
//         }
//         emitResult();
//     }
}

KIcon DebianScreenshotsJob::icon() const
{
    return KIcon("debian");
}

QList<KUrl> DebianScreenshotsJob::urls() const
{
    return m_urls;
}

} // namespace KIPIDebianScreenshotsPlugin
