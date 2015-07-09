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

#include "imgurtalker_common.h"

// Qt includes

#include <QWidget>
#include <QObject>
#include <QFileInfo>
#include <QSettings>

// KDE includes

#include <QUrl>
#include <kio/jobclasses.h>

// Libkipi includes

#include <KIPI/Interface>

namespace KIO
{
    class Job;
}

using namespace KIPI;

namespace KIPIImgurPlugin
{

// ----------------------------------------------------------------------

class ImgurTalker : public QWidget
{
    Q_OBJECT

public:

    enum State
    {
        IE_ADDPHOTO = 1,
        IE_REMOVEPHOTO,
        IE_LOGIN,
        IR_LOGOUT
    };

public:

    ImgurTalker(Interface* const iface, QWidget* const parent = 0);
    ~ImgurTalker();

    QUrl        currentUrl() const;
    void        setCurrentUrl(const QUrl& u);
    void        cancel();
    bool        imageRemove(const QString& hash);
    QUrl::List* imageQueue() const;
    void        imageUpload(const QUrl& filePath);
    void        parseResponse(const QByteArray& result);

Q_SIGNALS:
    void signalUploadProgress(int);
    void signalBusy(bool busy);
    void signalUploadStart(const QUrl& currentFile);
    void signalUploadDone(const QUrl& currentFile);
    void signalError(const QUrl& currentFile, const ImgurError& err);
    void signalSuccess(const QUrl& currentFile, const ImgurSuccess& success);
    void signalQueueChanged();

private:

    bool parseResponseImageUpload(const QByteArray& data);
    bool parseResponseImageRemove(const QByteArray& data);

protected Q_SLOTS:

    void slotResult(KJob* job);
    void slotData(KIO::Job* job, const QByteArray& data);
    void slotAddItems(const QUrl::List& list);
    void slotRemoveItems(const QUrl::List& list);
    void slotUploadDone(const QUrl& currentFile);
    void slotContinueUpload(bool yes);

protected:

    QUrl::List* m_queue;
    QUrl        m_currentUrl;
    State       m_state;

private:

    class Private;
    Private* const d;
};

} // namespace KIPIImgurPlugin

#endif // IMGURTALKER_H
