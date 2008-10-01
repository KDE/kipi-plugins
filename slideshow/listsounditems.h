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

#ifndef LISTSOUNDITEM_H
#define LISTSOUNDITEM_H

// QT includes
#include <QTime>
#include <QWidget>
#include <QString>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QListWidget>
#include <QListWidgetItem>

// Phonon includes
#include <Phonon/Global>
#include <Phonon/MediaObject>

// KDE includes
#include <kurl.h>

namespace KIPISlideShowPlugin
{

class SoundItem : public QObject, public QListWidgetItem
{

  Q_OBJECT

public:

    SoundItem(QListWidget* parent, KUrl &url);
    ~SoundItem();

    KUrl    url();
    QString artist();
    QString title();
    QTime   totalTime();
    void    setName(QString text);

signals:
    void totalTimeReady(KUrl,QTime);
    
private slots:

    void slotMediaStateChanged(Phonon::State newstate, Phonon::State oldstate);

private:

    KUrl                    m_url;
    QString                 m_artist;
    QString                 m_title;
    QTime                   m_totalTime;
    Phonon::MediaObject*    m_mediaObject;
};

class ListSoundItems : public QListWidget 
{
    Q_OBJECT

public:

    ListSoundItems(QWidget *parent=0);

signals:

    void addedDropItems(KUrl::List filesUrl);

protected:

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};

}  // NameSpace KIPISlideShowPlugin

#endif /* LISTSOUNDITEM_H */
