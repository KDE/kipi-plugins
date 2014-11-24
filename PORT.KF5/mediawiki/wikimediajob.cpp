/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Nathan Damie <nathan dot damie at gmail dot com>
 * Copyright (C) 2012      by Iliya Ivanov <ilko2002 at abv dot bg>
 * Copyright (C) 2012      by Peter Potrowl <peter dot potrowl at gmail dot com>
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
#include <QStringList>

// KDE includes

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>

// MediaWiki includes

#include <libmediawiki/upload.h>
#include <libmediawiki/mediawiki.h>

// KIPI includes

#include <libkipi/interface.h>

namespace KIPIWikiMediaPlugin
{

class WikiMediaJob::Private
{
public:

    Private()
    {
        interface = 0;
        mediawiki = 0;
    }

    KUrl::List                               urls;
    Interface*                               interface;
    MediaWiki*                               mediawiki;
    QString                                  error;
    QString                                  currentFile;
    QMap <QString, QMap <QString, QString> > imageDesc;
};

WikiMediaJob::WikiMediaJob(Interface* const interface, MediaWiki* const mediawiki, QObject* const parent)
    : KJob(parent), d(new Private)
{
    d->interface = interface;
    d->mediawiki = mediawiki;
}

WikiMediaJob::~WikiMediaJob()
{
    delete d;
}

void WikiMediaJob::start()
{
    QTimer::singleShot(0, this, SLOT(uploadHandle()));
}

void WikiMediaJob::begin()
{
    start();
}

void WikiMediaJob::setImageMap(const QMap <QString,QMap <QString,QString> >& imageDesc)
{
    d->imageDesc = imageDesc;
    kDebug() << "Map length:" << imageDesc.size();
}

void WikiMediaJob::uploadHandle(KJob* j)
{
    if(j != 0)
    {
        kDebug() << "Upload error" << j->error() << j->errorString() << j->errorText();
        emit uploadProgress(100);

        disconnect(j, SIGNAL(result(KJob*)), 
                   this, SLOT(uploadHandle(KJob*)));

        disconnect(j, SIGNAL(percent(KJob*,ulong)),
                   this, SLOT(slotUploadProgress(KJob*,ulong)));

        // Error from previous upload
        
        if((int)j->error() != 0)
        {
            const QString errorText = j->errorText();

            if(errorText.isEmpty())
            {
                d->error.append(i18n("Error on file '%1'\n", d->currentFile));
            }
            else
            {
                d->error.append(i18n("Error on file '%1': %2\n", d->currentFile, errorText));
            }
        }
    }

    // Upload next image
    
    if(!d->imageDesc.isEmpty())
    {
        QList<QString> keys        = d->imageDesc.keys();
        QMap<QString,QString> info = d->imageDesc.take(keys.first());
        Upload* const e1           = new Upload(*d->mediawiki, this);

        kDebug() << "Path:" << keys.first();

        QFile* const file = new QFile(keys.first(),this);
        file->open(QIODevice::ReadOnly);
        //emit fileUploadProgress(done = 0, total file.size());

        e1->setFile(file);
        d->currentFile = file->fileName();
        kDebug() << "Name:" << file->fileName();
        e1->setFilename(info["title"].replace(" ", "_"));
        kDebug() << "Title:" << info["title"];

        if(!info["comments"].isEmpty())
        {
            e1->setComment(info["comments"]);
        }
        else
        {
            e1->setComment(i18n("Uploaded via KIPI uploader"));
        }

        e1->setText(buildWikiText(info));
        keys.removeFirst();

        connect(e1, SIGNAL(result(KJob*)),
                this, SLOT(uploadHandle(KJob*)));

        connect(e1, SIGNAL(percent(KJob*,ulong)),
                this, SLOT(slotUploadProgress(KJob*,ulong)));

        emit uploadProgress(0);
        e1->start();
    }
    else
    {
        // Finish upload

        if(d->error.size() > 0)
        {
            KMessageBox::error(0,d->error);
        }
        else
        {
            emit endUpload();
        }

        d->error.clear();
    }
}

QString WikiMediaJob::buildWikiText(const QMap<QString, QString>& info) const
{
    QString text = QString::fromUtf8("=={{int:filedesc}}==");
    text.append("\n{{Information");
    text.append("\n|Description=").append(info["description"].toUtf8());
    text.append("\n|Source=");

    if(!info["source"].isEmpty())
    {
        text.append(info["source"].toUtf8());
    }

    text.append("\n|Author=");

    if(!info["author"].isEmpty())
    {
        text.append(info["author"].toUtf8());
    }

    text.append("\n|Date=").append(info["date"].toUtf8());
    text.append("\n|Permission=");
    text.append("\n|other_versions=");
    text.append("\n}}\n");

    QString latitude  = info["latitude"].toUtf8();
    QString longitude = info["longitude"].toUtf8();

    if(!latitude.isEmpty() && !longitude.isEmpty())
    {
        kDebug() << "Latitude:" << latitude << "; longitude:" << longitude;
        text.append("{{Coord").append("|").append(latitude).append("|").append(longitude).append("}}\n");
    }

    if(!info["genText"].isEmpty())
    {
        text.append(info["genText"].toUtf8()).append("\n");
    }

    if(!info["license"].isEmpty())
    {
        text.append("\n=={{int:license-header}}==\n");
        text.append(info["license"].toUtf8()).append("\n\n");
    }

    QStringList categories;

    if(!info["categories"].isEmpty())
    {
        categories = info["categories"].split("\n", QString::SkipEmptyParts);

        for(int i = 0; i < categories.size(); i++)
        {
            text.append("[[Category:").append(categories[i].toUtf8()).append("]]\n");
        }
    }

    if(!info["genCategories"].isEmpty())
    {
        categories = info["genCategories"].split("\n", QString::SkipEmptyParts);

        for(int i = 0; i < categories.size(); i++)
        {
            text.append("[[Category:").append(categories[i].toUtf8()).append("]]\n");
        }
    }

    return text;
}

void WikiMediaJob::slotUploadProgress(KJob* job, unsigned long percent)
{
    Q_UNUSED(job)
    emit uploadProgress((int)percent);
}

} // namespace KIPIWikiMediaPlugin
