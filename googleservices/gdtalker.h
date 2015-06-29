/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
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

#ifndef GD_TALKER_H
#define GD_TALKER_H

//Qt includes

#include <QList>
#include <QString>
#include <QObject>
#include <QStringList>

//KDE includes

#include <kio/job.h>

//local includes

#include "gsitem.h"
#include "authorize.h"

namespace KIPIGoogleServicesPlugin
{

class GDTalker : public Authorize
{
    Q_OBJECT

public:

    GDTalker(QWidget* const parent);
    ~GDTalker();

Q_SIGNALS:

    void signalListAlbumsDone(int, const QString&, const QList <GSFolder>&);
    void signalCreateFolderDone(int,const QString& msg);
    void signalAddPhotoDone(int,const QString& msg, const QString&);
    void signalSetUserName(const QString& msg);

private Q_SLOTS:
  
    void slotResult(KJob* job);

public:

    void getUserName();
    void listFolders();
    void createFolder(const QString& title,const QString& id);
    bool addPhoto(const QString& imgPath,const GSPhoto& info,const QString& id,bool rescale,int maxDim,int imageQuality);
    void cancel();

private:

    void parseResponseListFolders(const QByteArray& data);
    void parseResponseCreateFolder(const QByteArray& data);
    void parseResponseAddPhoto(const QByteArray& data);
    void parseResponseUserName(const QByteArray& data);

private:

    enum State
    {
        GD_LOGOUT = -1,
        GD_LISTFOLDERS = 0,
        GD_CREATEFOLDER,
        GD_ADDPHOTO,
        GD_USERNAME,
    };

private:

    QString      m_rootid;
    QString      m_rootfoldername;
    QString      m_username;
    State        m_state;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* GD_TALKER_H */
