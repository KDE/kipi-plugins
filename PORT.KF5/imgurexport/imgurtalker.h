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

#include <kurl.h>
#include <kio/jobclasses.h>

// LibKIPI includes

#include <libkipi/interface.h>

namespace KIO
{
    class Job;
}

using namespace KIPI;

namespace KIPIImgurExportPlugin
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

    KUrl        currentUrl() const;
    void        setCurrentUrl(const KUrl& u);
    void        cancel();
    bool        imageRemove(const QString& hash);
    KUrl::List* imageQueue() const;
    void        imageUpload(const KUrl& filePath);
    void        parseResponse(const QByteArray& result);

Q_SIGNALS:
    void signalUploadProgress(int);
    void signalBusy(bool busy);
    void signalUploadStart(const KUrl& currentFile);
    void signalUploadDone(const KUrl& currentFile);
    void signalError(const KUrl& currentFile, const ImgurError& err);
    void signalSuccess(const KUrl& currentFile, const ImgurSuccess& success);
    void signalQueueChanged();

private:

    bool parseResponseImageUpload(const QByteArray& data);
    bool parseResponseImageRemove(const QByteArray& data);

protected Q_SLOTS:

    void slotResult(KJob* job);
    void slotData(KIO::Job* job, const QByteArray& data);
    void slotAddItems(const KUrl::List& list);
    void slotRemoveItems(const KUrl::List& list);
    void slotUploadDone(const KUrl& currentFile);
    void slotContinueUpload(bool yes);

protected:

    KUrl::List* m_queue;
    KUrl        m_currentUrl;
    State       m_state;

private:

    class Private;
    Private* const d;
};

} // namespace KIPIImgurExportPlugin

#endif // IMGURTALKER_H
