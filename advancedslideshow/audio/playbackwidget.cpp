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

#include "playbackwidget.h"

// Qt includes

#include <QTime>
#include <QUrl>
#include <QKeyEvent>
#include <QIcon>

// Phonon includes

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/volumeslider.h>

// Local includes

#include "commoncontainer.h"

namespace KIPIAdvancedSlideshowPlugin
{

class PlaybackWidget::Private
{

public:

    Private()
    {
        sharedData   = 0;
        currIndex    = 0;
        mediaObject  = 0;
        volumeSlider = 0;
        audioOutput  = 0;
        stopCalled   = false;
        canHide      = true;
        isZeroTime   = false;
    }

    SharedContainer*      sharedData;
    QList<QUrl>           urlList;
    int                   currIndex;
    bool                  stopCalled;
    bool                  isZeroTime;
    bool                  canHide;

    Phonon::MediaObject*  mediaObject;
    Phonon::AudioOutput*  audioOutput;
    Phonon::VolumeSlider* volumeSlider;
};

PlaybackWidget::PlaybackWidget(QWidget* const parent, QList<QUrl>& urls, SharedContainer* const sharedData)
    : QWidget(parent),
      d(new Private)
{
    setupUi(this);

    d->sharedData   = sharedData;
    d->urlList      = urls;

    m_soundLabel->setPixmap(QIcon::fromTheme(QString::fromLatin1("speaker")).pixmap(64, 64));

    m_prevButton->setText(QString::fromLatin1(""));
    m_nextButton->setText(QString::fromLatin1(""));
    m_playButton->setText(QString::fromLatin1(""));
    m_stopButton->setText(QString::fromLatin1(""));

    m_prevButton->setIcon(QIcon::fromTheme(QString::fromLatin1("media-skip-backward")));
    m_nextButton->setIcon(QIcon::fromTheme(QString::fromLatin1("media-skip-forward")));
    m_playButton->setIcon(QIcon::fromTheme(QString::fromLatin1("media-playback-start")));
    m_stopButton->setIcon(QIcon::fromTheme(QString::fromLatin1("media-playback-stop")));

    connect(m_prevButton, SIGNAL(clicked()),
            this, SLOT(slotPrev()));

    connect(m_nextButton, SIGNAL(clicked()),
            this, SLOT(slotNext()));

    connect(m_playButton, SIGNAL(clicked()),
            this, SLOT(slotPlay()));

    connect(m_stopButton, SIGNAL(clicked()),
            this, SLOT(slotStop()));

    if (d->urlList.empty())
    {
        setEnabled(false);
        return;
    }

    // Waiting for files to be enqueued.
    m_playButton->setEnabled(false);
    m_prevButton->setEnabled(false);

    // Phonon
    d->mediaObject = new Phonon::MediaObject(this);
    d->mediaObject->setTransitionTime(1000);
    d->mediaObject->setTickInterval(500);

    connect(d->mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(slotMediaStateChanged(Phonon::State,Phonon::State)));

    connect(d->mediaObject, SIGNAL(finished()),
            this, SLOT(slotSongFinished()));

    connect(d->mediaObject, SIGNAL(tick(qint64)),
            this, SLOT(slotTimeUpdaterTimeout()));

    d->audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(d->mediaObject, d->audioOutput);

    d->volumeSlider = new Phonon::VolumeSlider(m_volumeWidget);
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_volumeWidget->sizePolicy().hasHeightForWidth());
    d->volumeSlider->setSizePolicy(sizePolicy1);
    d->volumeSlider->setProperty("maximum", QVariant(100));
    d->volumeSlider->setAudioOutput(d->audioOutput);
    d->volumeSlider->setOrientation(Qt::Horizontal);
    setZeroTime();

    // Loading first song
    d->mediaObject->setCurrentSource(static_cast<QUrl>(d->urlList[d->currIndex]));
}

PlaybackWidget::~PlaybackWidget()
{
    if (!d->urlList.empty())
    {
        d->mediaObject->stop();
    }

    delete d;
}

bool PlaybackWidget::canHide() const
{
    return d->canHide;
}

bool PlaybackWidget::isPaused() const
{
    return d->mediaObject->state() == Phonon::PausedState;
}

void PlaybackWidget::checkSkip()
{
    m_prevButton->setEnabled(true);
    m_nextButton->setEnabled(true);

    if ( !d->sharedData->soundtrackLoop )
    {
        if ( d->currIndex == 0 )
            m_prevButton->setEnabled(false);

        if ( d->currIndex == d->urlList.count() - 1 )
            m_nextButton->setEnabled(false);
    }
}

void PlaybackWidget::setGUIPlay(bool isPlaying)
{
    m_playButton->setIcon(QIcon::fromTheme(
        isPlaying ? QString::fromLatin1("media-playback-start") : QString::fromLatin1("media-playback-pause")));
}

void PlaybackWidget::setZeroTime()
{
    QTime zeroTime(0, 0, 0);
    m_elapsedTimeLabel->setText(zeroTime.toString(QString::fromLatin1("H:mm:ss")));
    m_totalTimeLabel->setText(zeroTime.toString(QString::fromLatin1("H:mm:ss")));
    d->isZeroTime = true;
}

void PlaybackWidget::enqueue(const QList<QUrl>& urls)
{
    d->urlList   = urls;
    d->currIndex = 0;

    if ( d->urlList.isEmpty() )
        return; // return on empty list

    d->mediaObject->setCurrentSource(static_cast<QUrl>(d->urlList[d->currIndex]));

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
    if ( d->mediaObject->state() == Phonon::PlayingState || d->mediaObject->state() == Phonon::BufferingState )
    {
        d->mediaObject->pause();
        setGUIPlay(true);
        d->canHide = false;
        emit signalPause();
        return;
    }

    if ( d->mediaObject->state() == Phonon::PausedState || d->mediaObject->state() == Phonon::StoppedState )
    {
        d->mediaObject->play();
        setGUIPlay(false);
        d->canHide = true;
        emit signalPlay();
    }
}

void PlaybackWidget::slotStop()
{
    d->mediaObject->stop();
    d->stopCalled = true;
    d->currIndex  = 0;
    d->mediaObject->setCurrentSource(static_cast<QUrl>(d->urlList[d->currIndex]));
    checkSkip();
    setGUIPlay(false);
    setZeroTime();
}

void PlaybackWidget::slotPrev()
{
    d->currIndex--;

    if ( d->currIndex < 0 )
    {
        if ( d->sharedData->soundtrackLoop )
        {
            d->currIndex = d->urlList.count() - 1;
        }
        else
        {
            d->currIndex = 0;
            return;
        }
    }

    setZeroTime();
    d->mediaObject->setCurrentSource(static_cast<QUrl>(d->urlList[d->currIndex]));
    d->mediaObject->play();
}

void PlaybackWidget::slotNext()
{
    d->currIndex++;

    if ( d->currIndex >= d->urlList.count() )
    {
        if ( d->sharedData->soundtrackLoop )
        {
            d->currIndex = 0;
        }
        else
        {
            d->currIndex = d->urlList.count() - 1;
            return;
        }
    }

    setZeroTime();
    d->mediaObject->setCurrentSource(static_cast<QUrl>(d->urlList[d->currIndex]));
    d->mediaObject->play();
}

void PlaybackWidget::slotTimeUpdaterTimeout()
{
    if ( d->mediaObject->state() == Phonon::ErrorState )
    {
        slotError();
        return;
    }

    long int current = d->mediaObject->currentTime();
    int hours        = (int)(current  / (long int)( 60 * 60 * 1000 ));
    int mins         = (int)((current / (long int)( 60 * 1000 )) - (long int)(hours * 60));
    int secs         = (int)((current / (long int)1000) - (long int)(hours * 60 + mins * 60));
    QTime elapsedTime(hours, mins, secs);

    if ( d->isZeroTime )
    {
        d->isZeroTime   = false;
        long int total = d->mediaObject->totalTime();
        hours          = (int)(total  / (long int)( 60 * 60 * 1000 ));
        mins           = (int)((total / (long int)( 60 * 1000 )) - (long int)(hours * 60));
        secs           = (int)((total / (long int)1000) - (long int)(hours * 60 + mins * 60));
        QTime totalTime(hours, mins, secs);
        m_totalTimeLabel->setText(totalTime.toString(QString::fromLatin1("H:mm:ss")));
    }

    m_elapsedTimeLabel->setText(elapsedTime.toString(QString::fromLatin1("H:mm:ss")));
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
                if ( d->stopCalled ) d->stopCalled = false;
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
    d->mediaObject->clearQueue();
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
