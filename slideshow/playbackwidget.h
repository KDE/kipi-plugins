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

#ifndef PLAYBACKWIDGET_H
#define PLAYBACKWIDGET_H

// QT includes
#include <QKeyEvent>

// Phonon includes
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <Phonon/Global>
#include <Phonon/VolumeSlider>

// KDE includes
#include <kurl.h>

// Local includes
#include "ui_playbackwidget.h"
#include "common.h"

namespace KIPISlideShowPlugin
{

class SharedData;

class PlaybackWidget : public QWidget, public Ui::PlaybackWidget
{
    Q_OBJECT

public:
    PlaybackWidget(QWidget*, KUrl::List&, SharedData* sharedData);
    ~PlaybackWidget();

    void enqueue(const KUrl::List);
    bool canHide() const;
    bool isPaused() const;
    void setPaused(bool);
    void keyPressEvent(QKeyEvent*);

private slots:
    void slotPlay( void );
    void slotStop( void );
    void slotPrev( void );
    void slotNext( void );
    void slotTimeUpdaterTimeout( void );
    void slotError( void );
    void slotMediaStateChanged(Phonon::State, Phonon::State);
    void slotSongFinished( void );

private:
    void checkSkip ( void );
    void setGUIPlay( bool );
    void setZeroTime( void);

signals:
    void signalPlay();
    void signalPause();

private:
    SharedData* m_sharedData;
    KUrl::List  m_urlList;
    int         m_currIndex;
    bool        m_muted;
    bool        m_stopCalled;
    bool        m_isZeroTime;

    Phonon::MediaObject*    m_mediaObject;
    Phonon::AudioOutput*    m_audioOutput;

    bool        m_canHide;
};

} // NameSpace KIPISlideShowPlugin

#endif // PLAYBACKWIDGET_H
