/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
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

#include "wikimediajob.moc"

// Qt includes

#include <QMessageBox>
#include <QFile>
#include <QTimer>

// KDE includes

#include <KMessageBox>
#include <KLocale>
#include <kdebug.h>

// Mediawiki includes

#include <libmediawiki/upload.h>
#include <libmediawiki/mediawiki.h>

// Local includes

#include "imageslist.h"

namespace KIPIWikiMediaPlugin
{

WikiMediaJob::WikiMediaJob(KIPI::Interface* interface, mediawiki::MediaWiki* mediawiki, QObject* parent)
    : KJob(parent), m_interface(interface), m_mediawiki(mediawiki)
{
}
void WikiMediaJob::start()
{
    QTimer::singleShot(0, this, SLOT(uploadHandle()));
}
void WikiMediaJob::begin()
{
    start();
}

void WikiMediaJob::setImageMap(QList<QMap<QString, QString> > imageDesc)
{
    m_imageDesc = imageDesc;
}

void WikiMediaJob::uploadHandle(KJob* j)
{
    if(j != 0)
    {
        kDebug() << "Upload" << (int)j->error();
        emit uploadProgress(100);

        disconnect(j, SIGNAL(result(KJob* )), 
                   this, SLOT(uploadHandle(KJob*)));

        disconnect(j, SIGNAL(percent(KJob *, unsigned long)),
                   this, SLOT(slotUploadProgress(KJob*,ulong)));

        //error from previous upload
        if((int)j->error() != 0)
        {
            m_error.append(i18n("Error on file : "));
            m_error.append(m_currentFile);
            if(j->errorText()==0){
                m_error.append(i18n(" : An error has occurred"));
            }
            m_error.append(" : "+ j->errorText());
            m_error.append('\n');
        }
    }
    //upload next image
    if(m_imageDesc.size() > 0)
    {
        QMap<QString,QString> info = m_imageDesc.takeFirst();
        mediawiki::Upload * e1     = new mediawiki::Upload( *m_mediawiki, this);

        kDebug() << "image path : " << info["url"].remove("file://");
        QFile* file = new QFile(info["url"].remove("file://"),this);
        file->open(QIODevice::ReadOnly);
        //emit fileUploadProgress(done = 0, total file.size());
        e1->setFile(file);
        m_currentFile=file->fileName();
        kDebug() << "image name : " << file->fileName().split("/").last();
        e1->setFilename(file->fileName());
        e1->setText(buildWikiText(info));

        connect(e1, SIGNAL(result(KJob* )),
                this, SLOT(uploadHandle(KJob*)));
        connect(e1, SIGNAL(percent(KJob *, unsigned long)),
                this, SLOT(slotUploadProgress(KJob*, ulong)));

        emit uploadProgress(0);
        e1->start();
    }
    else
    {
        //finish upload
        if(m_error.size() > 0)
        {
            KMessageBox::error(0,m_error);
        }
        else
        {
            emit endUpload();
        }
        m_error.clear();
    }
}

QString WikiMediaJob::buildWikiText(QMap<QString, QString> info)
{
    QString text;
    text.append(" == {{int:filedesc}} ==");
    text.append( "\n{{Information");
    text.append( "\n|Description=").append( info["description"]);
    text.append( "\n|Source=").append( "{{own}}");
    text.append( "\n|Author=");

    if(info.contains("author"))
    {
        text.append( "[[");
        text.append( info["author"]);
        text.append( "]]");
    }
    text.append( "\n|Date=").append( info["time"]);
    text.append( "\n|Permission=");
    text.append( "\n|other_versions=");

    text.append( "\n}}");
    QString altitude, longitude, latitude;

    if(info.contains("latitude")  ||
       info.contains("longitude") ||
       info.contains("altitude"))
    {
        if(info.contains("latitude"))
            latitude = info["latitude"];
        if(info.contains("longitude"))
            longitude = info["longitude"];
        if(info.contains("altitude"))
            altitude = info["altitude"];
    }

    if(!longitude.isEmpty() && !latitude.isEmpty())
    {
        text.append( "\n{{Location dec");
        text.append( "\n|").append( longitude );
        text.append( "\n|").append( latitude );
        text.append( "\n}}");
    }

    text.append( " == {{int:license}} ==");
    if(info.contains("licence"))
        text.append( info["licence"]);

    return text;
}

void WikiMediaJob::slotUploadProgress(KJob* job, unsigned long percent)
{
    Q_UNUSED(job)
    emit uploadProgress((int)percent);
}

} // namespace KIPIWikiMediaPlugin
