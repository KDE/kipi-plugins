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

#include "listsounditems.moc"

// Qt includes

#include <QString>
#include <QWidget>
#include <QEvent>
#include <QFileInfo>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QIcon>
#include <QBrush>

// KDE includes

#include <kurl.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>

namespace KIPIAdvancedSlideshowPlugin
{

SoundItem::SoundItem(QListWidget* const parent, const KUrl& url)
    : QListWidgetItem(parent)
{
    m_url = url;
    setIcon(SmallIcon("audio-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState));

    m_totalTime   = QTime(0, 0, 0);
    m_mediaObject = new Phonon::MediaObject();
    m_mediaObject->setCurrentSource(url);

    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(slotMediaStateChanged(Phonon::State,Phonon::State)));
}

SoundItem::~SoundItem()
{
}

KUrl SoundItem::url() const
{
    return m_url;
}

void SoundItem::setName(const QString& text)
{
    setText(text);
}

QString SoundItem::artist() const
{
    return m_artist;
}

QString SoundItem::title() const
{
    return m_title;
}

QTime SoundItem::totalTime() const
{
    return m_totalTime;
}

void SoundItem::slotMediaStateChanged(Phonon::State newstate, Phonon::State /*oldstate*/)
{
    if ( newstate == Phonon::ErrorState )
    {
        KMessageBox::detailedError( (QWidget*)(this),
                                    i18n("%1 is damaged and may not be playable.", m_url.fileName()),
                                    m_mediaObject->errorString(),
                                    i18n("Phonon error")
                                  );
        m_artist = m_url.fileName();
        m_title  = i18n("This file is damaged and may not be playable.");
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

    long int total = m_mediaObject->totalTime();
    int hours      = (int)(total / (long int)( 60 * 60 * 1000 ));
    int mins       = (int)((total / (long int)( 60 * 1000 )) - (long int)(hours * 60));
    int secs       = (int)((total / (long int)1000) - (long int)(hours * 60 * 60) - (long int)(mins * 60));
    m_totalTime    = QTime(hours, mins, secs);
    m_artist       = (m_mediaObject->metaData(Phonon::ArtistMetaData)).join(",");
    m_title        = (m_mediaObject->metaData(Phonon::TitleMetaData)).join(",");

    if ( m_artist.isEmpty() && m_title.isEmpty() )
        setText(m_url.fileName());
    else
        setText(i18nc("artist - title", "%1 - %2", artist(), title()));

    emit signalTotalTimeReady(m_url, m_totalTime);
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
    KUrl::List urls;

    foreach(const QUrl &url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
            urls.append(KUrl(url));
    }

    e->acceptProposedAction();

    if (!urls.isEmpty())
        emit signalAddedDropItems(urls);
}

KUrl::List ListSoundItems::fileUrls()
{
    KUrl::List files;

    for (int i = 0; i < count(); ++i)
    {
        SoundItem* const sitem = dynamic_cast<SoundItem*>(item(i));

        if (sitem)
        {
            files << KUrl(sitem->url());
        }
    }

    return files;
}

}  // namespace KIPIAdvancedSlideshowPlugin
