/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "listsounditems.h"

// Qt includes

#include <QString>
#include <QWidget>
#include <QEvent>
#include <QFileInfo>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QIcon>
#include <QBrush>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Phonon includes

#include <phonon/mediaobject.h>

namespace KIPIAdvancedSlideshowPlugin
{

class SoundItem::Private
{

public:

    Private()
    {
        mediaObject = 0;
    }

    QUrl                 url;
    QString              artist;
    QString              title;
    QTime                totalTime;
    Phonon::MediaObject* mediaObject;
};

SoundItem::SoundItem(QListWidget* const parent, const QUrl& url)
    : QListWidgetItem(parent),
      d(new Private)
{
    d->url = url;
    setIcon(QIcon::fromTheme(QString::fromLatin1("audio-x-generic")).pixmap(48, QIcon::Disabled));

    d->totalTime   = QTime(0, 0, 0);
    d->mediaObject = new Phonon::MediaObject();
    d->mediaObject->setCurrentSource(url);

    connect(d->mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(slotMediaStateChanged(Phonon::State,Phonon::State)));
}

SoundItem::~SoundItem()
{
    delete d;
}

QUrl SoundItem::url() const
{
    return d->url;
}

void SoundItem::setName(const QString& text)
{
    setText(text);
}

QString SoundItem::artist() const
{
    return d->artist;
}

QString SoundItem::title() const
{
    return d->title;
}

QTime SoundItem::totalTime() const
{
    return d->totalTime;
}

void SoundItem::slotMediaStateChanged(Phonon::State newstate, Phonon::State /*oldstate*/)
{
    if ( newstate == Phonon::ErrorState )
    {
        QMessageBox msgBox(QApplication::activeWindow());
        msgBox.setWindowTitle(i18n("Phonon error"));
        msgBox.setText(i18n("%1 is damaged and may not be playable.", d->url.fileName()));
        msgBox.setDetailedText(d->mediaObject->errorString());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();

        d->artist = d->url.fileName();
        d->title  = i18n("This file is damaged and may not be playable.");
        setText(i18nc("artist - title", "%1 - %2", artist(), title()));
        setBackground(QBrush(Qt::red));
        setForeground(QBrush(Qt::white));
        QFont errorFont = font();
        errorFont.setBold(true);
        errorFont.setItalic(true);
        setFont(errorFont);
        return;
    }

    if ( newstate != Phonon::StoppedState )
        return;

    long int total = d->mediaObject->totalTime();
    int hours      = (int)(total  / (long int)( 60 * 60 * 1000 ));
    int mins       = (int)((total / (long int)( 60 * 1000 )) - (long int)(hours * 60));
    int secs       = (int)((total / (long int)1000) - (long int)(hours * 60 * 60) - (long int)(mins * 60));
    d->totalTime   = QTime(hours, mins, secs);
    d->artist      = (d->mediaObject->metaData(Phonon::ArtistMetaData)).join(QString::fromLatin1(","));
    d->title       = (d->mediaObject->metaData(Phonon::TitleMetaData)).join(QString::fromLatin1(","));

    if ( d->artist.isEmpty() && d->title.isEmpty() )
        setText(d->url.fileName());
    else
        setText(i18nc("artist - title", "%1 - %2", artist(), title()));

    emit signalTotalTimeReady(d->url, d->totalTime);
}

// ------------------------------------------------------------------

ListSoundItems::ListSoundItems(QWidget* const parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setSortingEnabled(false);
    setIconSize(QSize(32, 32));
}

void ListSoundItems::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ListSoundItems::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void ListSoundItems::dropEvent(QDropEvent* e)
{
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    foreach(const QUrl &url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
            urls.append(QUrl(url));
    }

    e->acceptProposedAction();

    if (!urls.isEmpty())
        emit signalAddedDropItems(urls);
}

QList<QUrl> ListSoundItems::fileUrls()
{
    QList<QUrl> files;

    for (int i = 0; i < count(); ++i)
    {
        SoundItem* const sitem = dynamic_cast<SoundItem*>(item(i));

        if (sitem)
        {
            files << QUrl(sitem->url());
        }
    }

    return files;
}

}  // namespace KIPIAdvancedSlideshowPlugin
