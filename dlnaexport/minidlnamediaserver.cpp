/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2011 by Tuomo Penttinen <tp at herqq dot org>
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

#include "minidlnamediaserver.moc"

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>
#include <kurl.h>

// Qt includes

#include <QList>
#include <QFile>
#include <QProcess>
#include "wizard.h"

namespace KIPIDLNAExportPlugin
{

class MinidlnaServer::Private
{
public:

    Private()
    {
        port = "8200";
        name = "KIPI Plugin - DLNAExport";
        serial = "123456789";
        albumArt = "Cover.jpg/cover.jpg/AlbumArtSmall.jpg/albumartsmall.jpg/AlbumArt.jpg/albumart.jpg/Album.jpg/album.jpg/Folder.jpg/folder.jpg/Thumb.jpg/thumb.jpg";
        strictDLNA = "no";
        rootContainer = "P";
        modelNo = "1";
        filePath = "/home/smit/smit.conf";
    }

    QString port;
    QStringList directories;
    QString name;
    QString serial;
    QString albumArt;
    QString strictDLNA;
    QString rootContainer;
    QString modelNo;
    QString filePath;
};

MinidlnaServer::MinidlnaServer(QObject* const parent)
    : QObject(parent), d(new Private)
{
    
}

MinidlnaServer::~MinidlnaServer()
{
     delete d;
}

void MinidlnaServer::generateConfigFile()
{

    d->filePath = "/home/smit/smit.conf";
    QFile file(d->filePath);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    
    QTextStream out(&file);
    out <<"port="<< d->port <<"\n";
    
    foreach (QString directory, d->directories)
    {
        out <<"media_dir=P,"<< directory <<"\n";
    }
    
    out <<"friendly_name="<< d->name <<"\n";
    out <<"album_art_names="<< d->albumArt <<"\n";
    out <<"strict_dlna="<< d->strictDLNA <<"\n";
    out <<"serial="<< d->port <<"\n";
    out <<"model_number=" << d->modelNo << "\n";
    out <<"root_container=" << d->rootContainer << "\n";
    
    file.close();
  
}

void MinidlnaServer::setDirectories(const QStringList& directories)
{
    d->directories = directories;
}


void MinidlnaServer::startMinidlnaServer()
{
    QString program = "minidlna";
    QStringList arguments;
    arguments << "-d" << "-R" << "-f" << d->filePath;
    QProcess *minidlnaProcess = new QProcess(this);
    minidlnaProcess->start(program, arguments);
}

    
    
    
} // namespace KIPIDLNAExportPlugin
