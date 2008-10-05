/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

// QT includes
#include <q3strlist.h>
#include <q3dragobject.h>
#include <qstring.h>
#include <qwidget.h>
#include <qevent.h>
#include <qfileinfo.h>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QIcon>
#include <QBrush>

// KDE includes
#include <kurl.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klocale.h>

// Local includes
#include "listsounditems.h"
#include "listsounditems.moc"

namespace KIPISlideShowPlugin
{

SoundItem::SoundItem(QListWidget* parent, KUrl &url)
        : QListWidgetItem(parent)
{
    m_url = url;
    setIcon(SmallIcon( "audio-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState ));

    m_totalTime = QTime(0, 0, 0);
    m_mediaObject = new Phonon::MediaObject();
    m_mediaObject->setCurrentSource(url);
    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(slotMediaStateChanged(Phonon::State, Phonon::State)));
}

SoundItem::~SoundItem()
{
    delete m_mediaObject;
}

KUrl SoundItem::url()
{
    return m_url;
}

void SoundItem::setName(QString text)
{
    setText(text);
}

QString SoundItem::artist()
{
    return m_artist;
}

QString SoundItem::title()
{
    return m_title;
}


void SoundItem::slotMediaStateChanged(Phonon::State newstate, Phonon::State oldstate)
{
    // -- To please compiler --
    if ( oldstate ) {}

    // ------------------------

    if ( newstate == Phonon::ErrorState )
    {
        KMessageBox::detailedError( (QWidget*)(this),
                                    i18n("%1 is damaged and may not be playable.", m_url.fileName()),
                                    m_mediaObject->errorString(),
                                    i18n("Phonon error")
                                  );
        m_artist = m_url.fileName();
        m_title  = i18n("This file is damaged and may not be playable.");
        setText(artist().append(" - ").append(title()));
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

    int hours = (int)(total / (long int)( 60 * 60 * 1000 ));

    int mins  = (int)((total / (long int)( 60 * 1000 )) - (long int)(hours * 60));

    int secs  = (int)((total / (long int)1000) - (long int)(hours * 60 * 60) - (long int)(mins * 60));

    m_totalTime = QTime(hours, mins, secs);

    m_artist = (m_mediaObject->metaData(Phonon::ArtistMetaData)).join(",");

    m_title  = (m_mediaObject->metaData(Phonon::TitleMetaData)).join(",");

    if ( m_artist.isEmpty() && m_title.isEmpty() )
        setText(m_url.fileName());
    else
        setText(artist().append(" - ").append(title()));

    emit totalTimeReady(m_url, m_totalTime);
}

// ---------------------------------------------

ListSoundItems::ListSoundItems(QWidget *parent)
        : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setSortingEnabled(false);
    setIconSize(QSize(32, 32));
}

void ListSoundItems::dragEnterEvent(QDragEnterEvent *e)
{
    e->setAccepted(Q3UriDrag::canDecode(e));
}

void ListSoundItems::dropEvent(QDropEvent *e)
{
    Q3StrList strList;
    KUrl::List filesUrl;

    if ( !Q3UriDrag::decode(e, strList) ) return;

    Q3StrList stringList;

    Q3StrListIterator it(strList);

    char *str;

    while ( (str = it.current()) != 0 )
    {
        QString filePath = Q3UriDrag::uriToLocalFile(str);
        QFileInfo fileInfo(filePath);

        if (fileInfo.isFile() && fileInfo.exists())
        {
            KUrl url(fileInfo.filePath());
            filesUrl.append(url);
        }

        ++it;
    }

    if (filesUrl.isEmpty() == false)
        emit addedDropItems(filesUrl);
}

}  // NameSpace KIPISlideShowPlugin
