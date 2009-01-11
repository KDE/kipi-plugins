/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to export images to SmugMug web service
 *
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

#ifndef SMUGTALKER_H
#define SMUGTALKER_H

// Qt includes.
#include <QList>
#include <QString>
#include <QObject>

// KDE includes.
#include <KIO/Job>

// local includes.
#include "smugitem.h"

class QProgressDialog;

namespace KIPISmugExportPlugin
{

class SmugTalker : public QObject
{
    Q_OBJECT

public:
    SmugTalker(QWidget* parent);
    ~SmugTalker();

    SmugUser    getUser() const;

    bool    loggedIn();
    void    cancel();
    void    login(const QString& email, const QString& password);
    void    logout();

    void    listAlbums(const QString& nickName = "");
    void    listPhotos(int albumID, 
                       const QString& albumPassword = "", 
                       const QString& sitePassword = "");
    void    listAlbumTmpl();
    void    listCategories();
    void    listSubCategories(int categoryID);

    void    createAlbum(const SmugAlbum& album);

    bool    addPhoto(const QString& imgPath, int albumID);

public:
    QProgressDialog *m_authProgressDlg; // TODO: move to method?

signals:
    void signalBusy(bool val);
    void signalLoginDone(int errCode, const QString& errMsg);
    void signalAddPhotoDone(int errCode, const QString& errMsg);
    void signalCreateAlbumDone(int errCode, const QString& errMsg,
                               int newAlbumID);
    void signalListAlbumsDone(int errCode, const QString& errMsg,
                              const QList <SmugAlbum>& albumsList);
    void signalListPhotosDone(int errCode, const QString& errMsg,
                              const QList <SmugPhoto>& photosList);
    void signalListAlbumTmplDone(int errCode, const QString& errMsg,
                                 const QList <SmugAlbumTmpl>& albumTList);
    void signalListCategoriesDone(int errCode, const QString& errMsg,
                                  const QList <SmugCategory>& categoriesList);
    void signalListSubCategoriesDone(int errCode, const QString& errMsg,
                                     const QList <SmugCategory>& categoriesList);

private:
    enum State
    {
        SMUG_LOGIN = 0,
        SMUG_LOGOUT,
        SMUG_LISTALBUMS,
        SMUG_LISTPHOTOS,
        SMUG_LISTALBUMTEMPLATES,
        SMUG_LISTCATEGORIES,
        SMUG_LISTSUBCATEGORIES,
        SMUG_CREATEALBUM,
        SMUG_ADDPHOTO
    };

    QString errorToText(int errCode, const QString& errMsg);
    void parseResponseLogin(const QByteArray& data);
    void parseResponseLogout(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseCreateAlbum(const QByteArray& data);
    void parseResponseListAlbums(const QByteArray& data);
    void parseResponseListPhotos(const QByteArray& data);
    void parseResponseListAlbumTmpl(const QByteArray& data);
    void parseResponseListCategories(const QByteArray& data);
    void parseResponseListSubCategories(const QByteArray& data);

private slots:
    void data(KIO::Job *job, const QByteArray& data);
    void slotResult(KJob *job);

private:
    QWidget*   m_parent;

    QByteArray m_buffer;

    QString    m_userAgent;
    QString    m_apiURL;
    QString    m_apiVersion;
    QString    m_apiKey;
    QString    m_sessionID;

    SmugUser   m_user;

    KIO::Job*  m_job;

    State      m_state;
};

} // namespace KIPISmugExportPlugin

#endif /* SMUGTALKER_H */
