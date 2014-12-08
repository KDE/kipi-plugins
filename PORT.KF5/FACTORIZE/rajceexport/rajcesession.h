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

#ifndef KIPIRAJCEEXPORTPLUGIN_RAJCESESSION_H
#define KIPIRAJCEEXPORTPLUGIN_RAJCESESSION_H

// Qt includes

#include <QObject>
#include <QMutex>
#include <QQueue>

// KDE includes

#include <kio/global.h>

// Local includes

#include "sessionstate.h"

class QWidget;

class KJob;

namespace KIO
{
    class Job;
}

namespace KIPIRajceExportPlugin
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

    void data(KIO::Job* job, const QByteArray& data);
    void finished(KJob* job);
    void slotPercent(KJob* job, ulong percent);

private:

    void _startJob(RajceCommand*);
    void _enqueue(RajceCommand*);

private:

    QQueue<RajceCommand*> m_commandQueue;
    QMutex                m_queueAccess;
    QByteArray            m_buffer;
    QString               m_tmpDir;

    KJob*                 m_currentJob;

    SessionState          m_state;
};

} // namespace KIPIRajceExportPlugin

#endif // KIPIRAJCEEXPORTPLUGIN_RAJCESESSION_H
