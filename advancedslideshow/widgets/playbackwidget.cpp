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

#include "playbackwidget.moc"

// Qt includes

#include <QTime>
#include <QUrl>
#include <QKeyEvent>

// KDE includes

#include <kicon.h>

// Local includes

#include "commoncontainer.h"

namespace KIPIAdvancedSlideshowPlugin
{

PlaybackWidget::PlaybackWidget(QWidget* const parent, KUrl::List& urls, SharedContainer* const sharedData)
    : QWidget(parent)
{
    setupUi(this);

    m_sharedData  = sharedData;
    m_currIndex   = 0;
    m_mediaObject = 0;
    m_audioOutput = 0;
    m_urlList     = urls;
    m_stopCalled  = false;
    m_canHide     = true;
    m_isZeroTime  = false;

    m_soundLabel->setPixmap(KIcon("speaker").pixmap(64, 64));

    m_prevButton->setText("");
    m_nextButton->setText("");
    m_playButton->setText("");
    m_stopButton->setText("");

    m_prevButton->setIcon(KIcon("media-skip-backward"));
    m_nextButton->setIcon(KIcon("media-skip-forward"));
    m_playButton->setIcon(KIcon("media-playback-start"));
    m_stopButton->setIcon(KIcon("media-playback-stop"));

    connect(m_prevButton, SIGNAL(clicked()),
            this, SLOT(slotPrev()));

    connect(m_nextButton, SIGNAL(clicked()),
            this, SLOT(slotNext()));

    connect(m_playButton, SIGNAL(clicked()),
            this, SLOT(slotPlay()));

    connect(m_stopButton, SIGNAL(clicked()),
            this, SLOT(slotStop()));

    if (m_urlList.empty())
    {
        setEnabled(false);
        return;
    }

    // Waiting for files to be enqueued.
    m_playButton->setEnabled(false);

    m_prevButton->setEnabled(false);

    // Phonon
    m_mediaObject = new Phonon::MediaObject(this);
    m_mediaObject->setTransitionTime(1000);
    m_mediaObject->setTickInterval(500);

    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(slotMediaStateChanged(Phonon::State,Phonon::State)));

    connect(m_mediaObject, SIGNAL(finished()),
            this, SLOT(slotSongFinished()));

    connect(m_mediaObject, SIGNAL(tick(qint64)),
            this, SLOT(slotTimeUpdaterTimeout()));

    m_audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(m_mediaObject, m_audioOutput);
    m_volumeSlider->setAudioOutput(m_audioOutput);
    m_volumeSlider->setOrientation(Qt::Horizontal);
    setZeroTime();

    // Loading first song
    m_mediaObject->setCurrentSource(static_cast<QUrl>(m_urlList[m_currIndex]));
}

PlaybackWidget::~PlaybackWidget()
{
    if (!m_urlList.empty())
    {
        m_mediaObject->stop();
    }
}

bool PlaybackWidget::canHide() const
{
    return m_canHide;
}

bool PlaybackWidget::isPaused() const
{
    return m_mediaObject->state() == Phonon::PausedState;
}

void PlaybackWidget::checkSkip()
{
    m_prevButton->setEnabled(true);
    m_nextButton->setEnabled(true);

    if ( !m_sharedData->soundtrackLoop )
    {
        if ( m_currIndex == 0 )
            m_prevButton->setEnabled(false);

        if ( m_currIndex == m_urlList.count() - 1 )
            m_nextButton->setEnabled(false);
    }
}

void PlaybackWidget::setGUIPlay(bool isPlaying)
{
    m_playButton->setIcon(KIcon( isPlaying ? "media-playback-start" :
                                             "media-playback-pause" ));
}

void PlaybackWidget::setZeroTime()
{
    QTime zeroTime(0, 0, 0);
    m_elapsedTimeLabel->setText(zeroTime.toString("H:mm:ss"));
    m_totalTimeLabel->setText(zeroTime.toString("H:mm:ss"));
    m_isZeroTime = true;
}

void PlaybackWidget::enqueue(const KUrl::List& urls)
{
    m_urlList   = urls;
    m_currIndex = 0;

    if ( m_urlList.isEmpty() )
        return; // return on empty list

    m_mediaObject->setCurrentSource(static_cast<QUrl>(m_urlList[m_currIndex]));

    m_playButton->setEnabled(true);
}

void PlaybackWidget::setPaused(bool val)
{
    if (val == isPaused())
        return;

    slotPlay();
}

void PlaybackWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case(Qt::Key_Space):
        {
            m_playButton->animateClick();
            break;
        }

        case(Qt::Key_A):
        {
            if (m_prevButton->isEnabled())
                m_prevButton->animateClick();

            break;
        }

        case(Qt::Key_S):
        {
            if (m_nextButton->isEnabled())
                m_nextButton->animateClick();

            break;
        }

        case(Qt::Key_Escape):
        {
            if (m_stopButton->isEnabled())
                m_stopButton->animateClick();

            break;
        }

        default:
            break;
    }

    event->accept();
}

void PlaybackWidget::slotPlay()
{
    if ( m_mediaObject->state() == Phonon::PlayingState || m_mediaObject->state() == Phonon::BufferingState )
    {
        m_mediaObject->pause();
        setGUIPlay(true);
        m_canHide = false;
        emit signalPause();
        return;
    }

    if ( m_mediaObject->state() == Phonon::PausedState || m_mediaObject->state() == Phonon::StoppedState )
    {
        m_mediaObject->play();
        setGUIPlay(false);
        m_canHide = true;
        emit signalPlay();
    }
}

void PlaybackWidget::slotStop()
{
    m_mediaObject->stop();
    m_stopCalled = true;
    m_currIndex  = 0;
    m_mediaObject->setCurrentSource(static_cast<QUrl>(m_urlList[m_currIndex]));
    checkSkip();
    setGUIPlay(false);
    setZeroTime();
}

void PlaybackWidget::slotPrev()
{
    m_currIndex--;

    if ( m_currIndex < 0 )
    {
        if ( m_sharedData->soundtrackLoop )
        {
            m_currIndex = m_urlList.count() - 1;
        }
        else
        {
            m_currIndex = 0;
            return;
        }
    }

    setZeroTime();
    m_mediaObject->setCurrentSource(static_cast<QUrl>(m_urlList[m_currIndex]));
    m_mediaObject->play();
}

void PlaybackWidget::slotNext()
{
    m_currIndex++;

    if ( m_currIndex >= m_urlList.count() )
    {
        if ( m_sharedData->soundtrackLoop )
        {
            m_currIndex = 0;
        }
        else
        {
            m_currIndex = m_urlList.count() - 1;
            return;
        }
    }

    setZeroTime();
    m_mediaObject->setCurrentSource(static_cast<QUrl>(m_urlList[m_currIndex]));
    m_mediaObject->play();
}

void PlaybackWidget::slotTimeUpdaterTimeout()
{
    if ( m_mediaObject->state() == Phonon::ErrorState )
    {
        slotError();
        return;
    }

    long int current = m_mediaObject->currentTime();
    int hours        = (int)(current  / (long int)( 60 * 60 * 1000 ));
    int mins         = (int)((current / (long int)( 60 * 1000 )) - (long int)(hours * 60));
    int secs         = (int)((current / (long int)1000) - (long int)(hours * 60 + mins * 60));
    QTime elapsedTime(hours, mins, secs);

    if ( m_isZeroTime )
    {
        m_isZeroTime   = false;
        long int total = m_mediaObject->totalTime();
        hours          = (int)(total  / (long int)( 60 * 60 * 1000 ));
        mins           = (int)((total / (long int)( 60 * 1000 )) - (long int)(hours * 60));
        secs           = (int)((total / (long int)1000) - (long int)(hours * 60 + mins * 60));
        QTime totalTime(hours, mins, secs);
        m_totalTimeLabel->setText(totalTime.toString("H:mm:ss"));
    }

    m_elapsedTimeLabel->setText(elapsedTime.toString("H:mm:ss"));
}

void PlaybackWidget::slotMediaStateChanged(Phonon::State newstate, Phonon::State oldstate)
{
    switch (newstate)
    {
        case Phonon::StoppedState :
            m_playButton->setEnabled(true);
            setGUIPlay(true);

            if ( oldstate == Phonon::LoadingState )
            {
                if ( m_stopCalled ) m_stopCalled = false;
                else
                {
                    slotPlay();
                    checkSkip();
                }
            }
            break;

        case Phonon::ErrorState :
            slotError();
            break;

        case Phonon::PlayingState :
            setGUIPlay(false);
            checkSkip();
            break;

        case Phonon::PausedState :
            break;

        case Phonon::LoadingState :
            break;

        case Phonon::BufferingState :
            break;
    }
}

void PlaybackWidget::slotSongFinished()
{
    m_mediaObject->clearQueue();
    slotNext();
}

void PlaybackWidget::slotError()
{
    /* TODO :
     * Display error on slideshow.
     * A QWidget pop-up could help
     */

    slotNext();
}

} // namespace KIPIAdvancedSlideshowPlugin
