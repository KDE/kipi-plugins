/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef DBTALKER_H
#define DBTALKER_H

// Qt includes

#include <QList>
#include <QPair>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "dbitem.h"
#include "o2.h"
#include "o0globals.h"
#include "o0settingsstore.h"

using namespace KIPI;

namespace KIPIDropboxPlugin
{

class DBTalker : public QObject
{
    Q_OBJECT

public:

    DBTalker(QWidget* const parent);
    ~DBTalker();

public:

    void link();
    void unLink();
    bool authenticated();
    void getUserName();
    void cancel();
    void listFolders(const QString& path = QString());
    bool addPhoto(const QString& imgPath, const QString& uploadFolder, bool rescale, int maxDim, int imageQuality);
    void createFolder(const QString& path);

Q_SIGNALS:

    void signalBusy(bool val);
    void signalLinkingSucceeded();
    void signalLinkingFailed();
    void signalSetUserName(const QString& msg);
    void signalListAlbumsFailed(const QString& msg);
    void signalListAlbumsDone(const QList<QPair<QString, QString> >& list);
    void signalCreateFolderFailed(const QString& msg);
    void signalCreateFolderSucceeded();
    void signalAddPhotoFailed(const QString& msg);
    void signalAddPhotoSucceeded();

private Q_SLOTS:

    void slotLinkingFailed();
    void slotLinkingSucceeded();
    void slotOpenBrowser(const QUrl& url); 
    void slotFinished(QNetworkReply* reply);

private:

    void parseResponseUserName(const QByteArray& data);
    void parseResponseListFolders(const QByteArray& data);
    void parseResponseCreateFolder(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);

private:

    enum State
    {
        DB_USERNAME = 0,
        DB_LISTFOLDERS,
        DB_CREATEFOLDER,
        DB_ADDPHOTO
    };

private:

    QString                m_apikey;
    QString                m_secret;
    QString                m_authUrl;
    QString                m_tokenUrl;

    QWidget*               m_parent;

    QNetworkAccessManager* m_netMngr;

    QNetworkReply*         m_reply;

    QSettings*             m_settings;

    State                  m_state;

    QByteArray             m_buffer;

    Interface*             m_iface;

    MetadataProcessor*     m_meta;

    O2*                    m_o2;
    O0SettingsStore*       m_store;
};

} // namespace KIPIDropboxPlugin

#endif /*DBTALKER_H*/
