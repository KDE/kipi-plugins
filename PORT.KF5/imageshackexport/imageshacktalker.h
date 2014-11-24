/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#ifndef IMAGESHACKTALKER_H
#define IMAGESHACKTALKER_H

// Qt includes

#include <QObject>
#include <QString>
#include <QMap>

// KDE includes

#include <kio/job.h>

class QDomElement;
class QByteArray;

namespace KIPIImageshackExportPlugin
{

class Imageshack;

struct ImageshackImageInfo
{
};

class ImageshackTalker : public QObject
{
    Q_OBJECT

public:

    ImageshackTalker(Imageshack* imghack);
    ~ImageshackTalker();

    bool loggedIn();

    void authenticate();
    void cancelLogIn();
    void cancel();
    void getGalleries();

    void uploadItem(QString path, QMap<QString, QString> opts);
    void uploadItemToGallery(QString path, const QString& gallery, QMap<QString, QString> opts);

Q_SIGNALS:

    void signalNeedRegistrationCode();
    void signalBusy(bool busy);
    void signalJobInProgress(int step, int maxStep = 0, const QString& label = QString());
    void signalLoginDone(int errCode,  const QString &errMsg);
    void signalGetGalleriesDone(int errCode, const QString &errMsg);

    void signalAddPhotoDone(int errCode, const QString& errMsg);
    void signalUpdateGalleries(const QStringList& gTexts, const QStringList& gNames);

private:

    enum State
    {
        IMGHCK_DONOTHING,
        IMGHCK_CHECKREGCODE,
        IMGHCK_GETGALLERIES,
        IMGHCK_ADDPHOTO,
        IMGHCK_ADDVIDEO,
        IMGHCK_ADDPHOTOGALLERY
    };

    enum Step
    {
        STEP_UPLOADITEM,
        STEP_CREATEGALLERY,
        STEP_ADDITEMTOGALLERY
    };

private Q_SLOTS:

    void data(KIO::Job* job, const QByteArray& data);
    void slotResult(KJob* job);

private:

    QString getCallString(QMap<QString, QString>& args);
    void checkRegistrationCode();
    void parseCheckRegistrationCode(const QByteArray& data);
    void parseGetGalleries(const QByteArray& data);
    void checkRegistrationCodeDone(int errCode, const QString& errMsg);
    void authenticationDone(int errCode, const QString& errMsg);

    void logOut();

    int parseErrorResponse(QDomElement elem, QString& errMsg);

    void parseUploadPhotoDone(QByteArray data);
    void parseAddPhotoToGalleryDone(QByteArray data);

    QString mimeType(const QString& path);

private:

    Imageshack* m_imageshack;

    QByteArray  m_buffer;

    QString     m_userAgent;
    KUrl        m_photoApiUrl;
    KUrl        m_videoApiUrl;
    KUrl        m_loginApiUrl;
    KUrl        m_galleryUrl;
    QString     m_appKey;

    bool        m_loginInProgress;

    KIO::Job*   m_job;

    State       m_state;
};

} // namespace KIPIImageshackExportPlugin

#endif // IMAGESHACKTALKER_H
