/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2009 by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef SWCONNECTOR_H
#define SWCONNECTOR_H

// Qt includes

#include <QList>
#include <QString>
#include <QTime>
#include <QObject>

// KDE includes

#include <KIO/Job>

// local includes

#include "switem.h"

class QDomElement;

namespace KIPIShwupPlugin
{

class SwConnector : public QObject
{
    Q_OBJECT

public:

    SwConnector(QWidget* parent);
    ~SwConnector();

    SwUser  getUser() const;
    void    setUser(SwUser user);
    bool    isLoggedIn();
    void    logout();

    void    getRestServiceURL();
    void    listAlbums();
    void    createAlbum(const SwAlbum& album);
    bool    addPhoto(const QString& imgPath, long long albumID, 
                     const QString& caption);
 
    void    cancel();

Q_SIGNALS:

    void signalBusy(bool val);

    void signalShwupKipiBlackListed() const;
    void signalShwupSignatureError() const;
    void signalShwupInvalidCredentials() const;

    void signalRequestRestURLDone(int errCode, const QString& errMsg);
    void signalListAlbumsDone(int errCode, const QString& errMsg,
                              const QList <SwAlbum>& albumsList);
    void signalCreateAlbumDone(int errCode, const QString& errMsg,
                               const SwAlbum& newAlbum);
    void signalAddPhotoDone(int errCode, const QString& errMsg);

private:

    void setupRequest(KIO::TransferJob* job, const QString& requestPath, const QString& method, 
                      const QString& md5, const QString& type, const QString& length, bool needsPassword);

    QDomElement getResponseDoc(KIO::Job* job, const QByteArray &data) const;
    void requestRestURLResultHandler(KIO::Job* job, const QByteArray &data);
    void listAlbumsResultHandler(KIO::Job* job, const QByteArray &data);
    void createAlbumResultHandler(KIO::Job* job, const QByteArray &data);
    void addPhotoResultHandler(KIO::Job* job, const QByteArray &data);

private Q_SLOTS:

    void slotRequestRestURLRedirection(KIO::Job* job, const KUrl& newUrl);
    void data(KIO::Job *job, const QByteArray& data);
    void slotResult(KJob *job);

private:

    void (SwConnector::*m_resultHandler)(KIO::Job* job, const QByteArray& data);

private:

    QWidget*   m_parent;

    QByteArray m_buffer;

    QString    m_userAgent;
    QString    m_apiStartURL;
    QString    m_apiDomainURL;
    QString    m_apiRestPath;
    QString    m_apiVersion;
    QString    m_apiKey;
    QString    m_apiSecretKey;

    SwUser     m_user;
    bool       m_loggedIn;

    KIO::Job*  m_job;
};

} // namespace KIPIFacebookPlugin

#endif /* SWCONNECTOR_H */
