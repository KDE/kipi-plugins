/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#ifndef RAJCESESSION_H
#define RAJCESESSION_H

// Qt includes

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QNetworkReply>
#include <QNetworkAccessManager>

// Local includes

#include "sessionstate.h"

class QWidget;

namespace KIPIRajcePlugin
{

class RajceCommand;

class RajceSession : public QObject
{
    Q_OBJECT

public:

    explicit RajceSession(QWidget* const jobParent, const QString& tmpDir);

    void  init(const SessionState& initialState);
    const SessionState& state() const;
    void  login(const QString& username, const QString& password);
    void  logout();

    void loadAlbums();
    void createAlbum(const QString& name, const QString& description, bool visible);
    void openAlbum(const Album& album);
    void closeAlbum();

    void uploadPhoto(const QString& path, unsigned dimension, int jpgQuality);

    void clearLastError();
    void cancelCurrentCommand();

Q_SIGNALS:

    void busyStarted(unsigned);
    void busyFinished(unsigned);
    void busyProgress(unsigned, unsigned percent);

private Q_SLOTS:

    void slotFinished(QNetworkReply* reply);
    void slotUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:

    void _startJob(RajceCommand*);
    void _enqueue(RajceCommand*);

private:

    QQueue<RajceCommand*>  m_commandQueue;
    QMutex                 m_queueAccess;
    QString                m_tmpDir;

    QNetworkAccessManager* m_netMngr;
    QNetworkReply*         m_reply;

    SessionState           m_state;
};

} // namespace KIPIRajcePlugin

#endif // RAJCESESSION_H
