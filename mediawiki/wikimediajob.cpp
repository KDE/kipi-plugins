/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Nathan Damie <nathan dot damie at gmail dot com>
 * Copyright (C) 2012      by Iliya Ivanov <iliya dot ivanov at etudiant dot univ dash lille1 dot fr>
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

// Mediawiki includes

#include <libmediawiki/upload.h>
#include <libmediawiki/mediawiki.h>

// KIPI includes

#include <libkipi/interface.h>

namespace KIPIWikiMediaPlugin
{

WikiMediaJob::WikiMediaJob(Interface* const interface, MediaWiki* const mediawiki, QObject* const parent)
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

void WikiMediaJob::setImageMap(const QMap <QString,QMap <QString,QString> >& imageDesc)
{
    m_imageDesc = imageDesc;
}

void WikiMediaJob::uploadHandle(KJob* j)
{
    if(j != 0)
    {
        kDebug() << "Upload" << (int)j->error();
        emit uploadProgress(100);

        disconnect(j, SIGNAL(result(KJob*)), 
                   this, SLOT(uploadHandle(KJob*)));

        disconnect(j, SIGNAL(percent(KJob*, ulong)),
                   this, SLOT(slotUploadProgress(KJob*, ulong)));

        //error from previous upload
        if((int)j->error() != 0)
        {
            const QString errorText = j->errorText();
            if(errorText.isEmpty())
            {
                m_error = i18n("Error on file '%1'\n", m_currentFile);
            }
            else
            {
                m_error = i18n("Error on file '%1': %2\n", m_currentFile, errorText.isEmpty());
            }
        }
    }

    // upload next image
    if(!m_imageDesc.isEmpty())
    {
        QList<QString> keys        = m_imageDesc.keys();
        QMap<QString,QString> info = m_imageDesc.take(keys.first());
        Upload* e1                 = new Upload(*m_mediawiki, this);

        kDebug() << "image path : " << keys.first();

        QFile* file = new QFile(keys.first(),this);
        file->open(QIODevice::ReadOnly);
        //emit fileUploadProgress(done = 0, total file.size());

        e1->setFile(file);
        m_currentFile = file->fileName();
        kDebug() << "image name : " << file->fileName();
        e1->setFilename(info["title"]);
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

QString WikiMediaJob::buildWikiText(const QMap<QString, QString>& info)
{
    QString text = QString::fromUtf8("=={{int:filedesc}}==");
    text.append("\n{{Information");
    text.append("\n|Description=").append(info["description"].toUtf8());
    text.append("\n|Source=").append("{{own}}");
    text.append("\n|Author=");

    if(!info["author"].isEmpty())
    {
        text.append("[[");
        text.append(info["author"].toUtf8());
        text.append("]]");
    }

    text.append("\n|Date=").append(info["time"].toUtf8());
    text.append("\n|Permission=");
    text.append("\n|other_versions=");
    text.append("\n}}");

    QString latitude  = info["latitude"].toUtf8();
    QString longitude = info["longitude"].toUtf8();

    if(!latitude.isEmpty() && !longitude.isEmpty())
    {
        kDebug() << "Latitude: \"" << latitude << "\"; longitude: \"" << longitude << "\"";
        text.append("\n{{Location dec").append("|").append(latitude).append("|").append(longitude).append("}}");
    }

    text.append("\n\n=={{int:license-header}}==\n");
    text.append(info["license"].toUtf8()).append("\n");

    if(!info["categories"].isEmpty())
    {
        text.append("\n[[Category:Uploaded with KIPI uploader]]\n");
        QStringList categories = info["categories"].split("\n", QString::SkipEmptyParts);

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
