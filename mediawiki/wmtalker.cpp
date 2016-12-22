/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "wmtalker.h"

// Qt includes

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <QStringList>

// KDE includes

#include <klocalizedstring.h>

// MediaWiki includes

#include <MediaWiki/Upload>
#include <MediaWiki/MediaWiki>

// KIPI includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIWikiMediaPlugin
{

class WMTalker::Private
{
public:

    Private()
    {
        interface = 0;
        mediawiki = 0;
    }

    QList<QUrl>                              urls;
    Interface*                               interface;
    MediaWiki*                               mediawiki;
    QString                                  error;
    QString                                  currentFile;
    QMap <QString, QMap <QString, QString> > imageDesc;
};

WMTalker::WMTalker(Interface* const interface, MediaWiki* const mediawiki, QObject* const parent)
    : KJob(parent),
      d(new Private)
{
    d->interface = interface;
    d->mediawiki = mediawiki;
}

WMTalker::~WMTalker()
{
    delete d;
}

void WMTalker::start()
{
    QTimer::singleShot(0, this, SLOT(uploadHandle()));
}

void WMTalker::begin()
{
    start();
}

void WMTalker::setImageMap(const QMap <QString,QMap <QString,QString> >& imageDesc)
{
    d->imageDesc = imageDesc;
    qCDebug(KIPIPLUGINS_LOG) << "Map length:" << imageDesc.size();
}

void WMTalker::uploadHandle(KJob* j)
{
    if (j != 0)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Upload error" << j->error() << j->errorString() << j->errorText();
        emit uploadProgress(100);

        disconnect(j, SIGNAL(result(KJob*)),
                   this, SLOT(uploadHandle(KJob*)));

        disconnect(j, SIGNAL(percent(KJob*, ulong)),
                   this, SLOT(slotUploadProgress(KJob*, ulong)));

        // Error from previous upload

        if ((int)j->error() != 0)
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

    if (!d->imageDesc.isEmpty())
    {
        QList<QString> keys        = d->imageDesc.keys();
        QMap<QString,QString> info = d->imageDesc.take(keys.first());
        Upload* const e1           = new Upload(*d->mediawiki, this);

        qCDebug(KIPIPLUGINS_LOG) << "Path:" << keys.first();

        QFile* const file = new QFile(keys.first(),this);
        file->open(QIODevice::ReadOnly);
        //emit fileUploadProgress(done = 0, total file.size());

        e1->setFile(file);
        d->currentFile = file->fileName();
        qCDebug(KIPIPLUGINS_LOG) << "Name:" << file->fileName();
        e1->setFilename(info[QLatin1String("title")].replace(QLatin1String(" "), QLatin1String("_")));
        qCDebug(KIPIPLUGINS_LOG) << "Title:" << info[QLatin1String("title")];

        if (!info[QLatin1String("comments")].isEmpty())
        {
            e1->setComment(info[QLatin1String("comments")]);
        }
        else
        {
            e1->setComment(i18n("Uploaded via KIPI uploader"));
        }

        e1->setText(buildWikiText(info));
        keys.removeFirst();

        connect(e1, SIGNAL(result(KJob*)),
                this, SLOT(uploadHandle(KJob*)));

        connect(e1, SIGNAL(percent(KJob*, ulong)),
                this, SLOT(slotUploadProgress(KJob*, ulong)));

        emit uploadProgress(0);
        e1->start();
    }
    else
    {
        // Finish upload

        if(d->error.size() > 0)
        {
            QMessageBox::critical(QApplication::activeWindow(), i18n("Error"), d->error);
        }
        else
        {
            emit endUpload();
        }

        d->error.clear();
    }
}

QString WMTalker::buildWikiText(const QMap<QString, QString>& info) const
{
    QString text = QString::fromUtf8("=={{int:filedesc}}==");
    text.append(QLatin1String("\n{{Information"));
    text.append(QLatin1String("\n|Description=")).append(info[QLatin1String("description")]);
    text.append(QLatin1String("\n|Source="));

    if (!info[QLatin1String("source")].isEmpty())
    {
        text.append(info[QLatin1String("source")]);
    }

    text.append(QLatin1String("\n|Author="));

    if (!info[QLatin1String("author")].isEmpty())
    {
        text.append(info[QLatin1String("author")]);
    }

    text.append(QLatin1String("\n|Date=")).append(info[QLatin1String("date")]);
    text.append(QLatin1String("\n|Permission="));
    text.append(QLatin1String("\n|other_versions="));
    text.append(QLatin1String("\n}}\n"));

    QString latitude  = info[QLatin1String("latitude")];
    QString longitude = info[QLatin1String("longitude")];

    if (!latitude.isEmpty() && !longitude.isEmpty())
    {
        qCDebug(KIPIPLUGINS_LOG) << "Latitude:" << latitude << "; longitude:" << longitude;
        text.append(QLatin1String("{{Location|")).append(latitude).append(QLatin1String("|")).append(longitude).append(QLatin1String("}}\n"));
    }

    if (!info[QLatin1String("genText")].isEmpty())
    {
        text.append(info[QLatin1String("genText")]).append(QLatin1String("\n"));
    }

    if (!info[QLatin1String("license")].isEmpty())
    {
        text.append(QLatin1String("\n=={{int:license-header}}==\n"));
        text.append(info[QLatin1String("license")]).append(QLatin1String("\n\n"));
    }

    QStringList categories;

    if (!info[QLatin1String("categories")].isEmpty())
    {
        categories = info[QLatin1String("categories")].split(QLatin1String("\n"), QString::SkipEmptyParts);

        for(int i = 0; i < categories.size(); i++)
        {
            text.append(QLatin1String("[[Category:")).append(categories[i]).append(QLatin1String("]]\n"));
        }
    }

    if (!info[QLatin1String("genCategories")].isEmpty())
    {
        categories = info[QLatin1String("genCategories")].split(QLatin1String("\n"), QString::SkipEmptyParts);

        for(int i = 0; i < categories.size(); i++)
        {
            text.append(QLatin1String("[[Category:")).append(categories[i]).append(QLatin1String("]]\n"));
        }
    }

    return text;
}

void WMTalker::slotUploadProgress(KJob* job, unsigned long percent)
{
    Q_UNUSED(job)
    emit uploadProgress((int)percent);
}

} // namespace KIPIWikiMediaPlugin
