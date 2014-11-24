/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-05
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Partially based on Renchi Raju's ToolBar class.
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

#include "slideplaybackwidget.moc"

// Qt includes

#include <QToolButton>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPixmap>

// KDE includes

#include <kapplication.h>
#include <kicon.h>
#include <klocale.h>
#include <kdebug.h>

namespace KIPIAdvancedSlideshowPlugin
{
SlidePlaybackWidget::SlidePlaybackWidget(QWidget* const parent)
    : QWidget(parent)
{
    setupUi(this);
    m_playButton->setCheckable(true);
    m_slideLabel->setPixmap(KIcon("view-presentation").pixmap(64, 64));

    m_prevButton->setText("");
    m_nextButton->setText("");
    m_playButton->setText("");
    m_stopButton->setText("");

    m_prevButton->setIcon(KIcon("media-skip-backward"));
    m_nextButton->setIcon(KIcon("media-skip-forward"));
    m_playButton->setIcon(KIcon("media-playback-start"));
    m_stopButton->setIcon(KIcon("media-playback-stop"));

    m_canHide = true;

    connect(m_playButton, SIGNAL(toggled(bool)),
            this, SLOT(slotPlayButtonToggled()));

    connect(m_nextButton, SIGNAL(clicked()),
            this, SLOT(slotNexPrevClicked()));

    connect(m_prevButton, SIGNAL(clicked()),
            this, SLOT(slotNexPrevClicked()));

    connect(m_nextButton, SIGNAL(clicked()),
            this, SIGNAL(signalNext()));

    connect(m_prevButton, SIGNAL(clicked()),
            this, SIGNAL(signalPrev()));

    connect(m_stopButton, SIGNAL(clicked()),
            this, SIGNAL(signalClose()));

    slotPlayButtonToggled();
}

SlidePlaybackWidget::~SlidePlaybackWidget()
{
}

bool SlidePlaybackWidget::canHide() const
{
    return m_canHide;
}

bool SlidePlaybackWidget::isPaused() const
{
    return m_playButton->isChecked();
}

void SlidePlaybackWidget::setPaused(bool val)
{
    if (val == isPaused())
        return;

    m_playButton->setChecked(val);

    slotPlayButtonToggled();
}

void SlidePlaybackWidget::setEnabledPlay(bool val)
{
    m_playButton->setEnabled(val);
}

void SlidePlaybackWidget::setEnabledNext(bool val)
{
    m_nextButton->setEnabled(val);
}

void SlidePlaybackWidget::setEnabledPrev(bool val)
{
    m_prevButton->setEnabled(val);
}

void SlidePlaybackWidget::slotPlayButtonToggled()
{
    if (m_playButton->isChecked())
    {
        m_canHide                 = false;
        KIconLoader* const loader = KIconLoader::global();
        m_playButton->setIcon(loader->loadIcon("media-playback-start", KIconLoader::NoGroup, 22));
        emit signalPause();
    }
    else
    {
        m_canHide                 = true;
        KIconLoader* const loader = KIconLoader::global();
        m_playButton->setIcon(loader->loadIcon("media-playback-pause", KIconLoader::NoGroup, 22));
        emit signalPlay();
    }
}

void SlidePlaybackWidget::slotNexPrevClicked()
{
    if (!m_playButton->isChecked())
    {
        m_playButton->setChecked(true);
        m_canHide                 = false;
        KIconLoader* const loader = KIconLoader::global();
        m_playButton->setIcon(loader->loadIcon("media-playback-start", KIconLoader::NoGroup, 22));
        emit signalPause();
    }
}

void SlidePlaybackWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case(Qt::Key_Space):
        {
            if (m_playButton->isEnabled())
                m_playButton->animateClick();

            break;
        }

        case(Qt::Key_PageUp):
        {
            if (m_prevButton->isEnabled())
                m_prevButton->animateClick();

            break;
        }

        case(Qt::Key_PageDown):
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

} // namespace KIPIAdvancedSlideshowPlugin
