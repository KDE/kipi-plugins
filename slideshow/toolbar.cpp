/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-05
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

// Qt includes.

#include <QToolButton>
#include <QHBoxLayout>
#include <QKeyEvent>

// KDE includes.

#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>

// Local includes.

#include "toolbar.h"
#include "toolbar.moc"

namespace KIPISlideShowPlugin
{

ToolBar::ToolBar(QWidget* parent)
        : QWidget(parent)
{
    QHBoxLayout* lay = new QHBoxLayout();
    setLayout(lay);
    m_playBtn = new QToolButton(this);
    m_prevBtn = new QToolButton(this);
    m_nextBtn = new QToolButton(this);
    m_stopBtn = new QToolButton(this);
    m_playBtn->setCheckable(true);

    KIconLoader* loader = KIconLoader::global();
    m_playBtn->setIcon(loader->loadIcon("media-playback-pause", KIconLoader::NoGroup, 22));
    m_prevBtn->setIcon(loader->loadIcon("media-skip-backward", KIconLoader::NoGroup, 22));
    m_nextBtn->setIcon(loader->loadIcon("media-skip-forward", KIconLoader::NoGroup, 22));
    m_stopBtn->setIcon(loader->loadIcon("media-playback-stop", KIconLoader::NoGroup, 22));

    lay->addWidget(m_playBtn);
    lay->addWidget(m_prevBtn);
    lay->addWidget(m_nextBtn);
    lay->addWidget(m_stopBtn);

    adjustSize();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_canHide = true;

    connect(m_playBtn, SIGNAL(toggled(bool)),
            this, SLOT(slotPlayBtnToggled()));

    connect(m_nextBtn, SIGNAL(clicked()),
            this, SLOT(slotNexPrevClicked()));

    connect(m_prevBtn, SIGNAL(clicked()),
            this, SLOT(slotNexPrevClicked()));

    connect(m_nextBtn, SIGNAL(clicked()),
            this, SIGNAL(signalNext()));

    connect(m_prevBtn, SIGNAL(clicked()),
            this, SIGNAL(signalPrev()));

    connect(m_stopBtn, SIGNAL(clicked()),
            this, SIGNAL(signalClose()));
}

ToolBar::~ToolBar()
{
}

bool ToolBar::canHide() const
{
    return m_canHide;
}

bool ToolBar::isPaused() const
{
    return m_playBtn->isChecked();
}

void ToolBar::setPaused(bool val)
{
    if (val == isPaused())
        return;

    m_playBtn->setChecked(val);

    slotPlayBtnToggled();
}

void ToolBar::setEnabledPlay(bool val)
{
    m_playBtn->setEnabled(val);
}

void ToolBar::setEnabledNext(bool val)
{
    m_nextBtn->setEnabled(val);
}

void ToolBar::setEnabledPrev(bool val)
{
    m_prevBtn->setEnabled(val);
}

void ToolBar::slotPlayBtnToggled()
{
    if (m_playBtn->isChecked())
    {
        m_canHide = false;
        KIconLoader* loader = KIconLoader::global();
        m_playBtn->setIcon(loader->loadIcon("media-playback-start", KIconLoader::NoGroup, 22));
        emit signalPause();
    }
    else
    {
        m_canHide = true;
        KIconLoader* loader = KIconLoader::global();
        m_playBtn->setIcon(loader->loadIcon("media-playback-pause", KIconLoader::NoGroup, 22));
        emit signalPlay();
    }
}

void ToolBar::slotNexPrevClicked()
{
    if (!m_playBtn->isChecked())
    {
        m_playBtn->setChecked(true);
        m_canHide = false;
        KIconLoader* loader = KIconLoader::global();
        m_playBtn->setIcon(loader->loadIcon("media-playback-start", KIconLoader::NoGroup, 22));
        emit signalPause();
    }
}

void ToolBar::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case(Qt::Key_Space):
        {
            if (m_playBtn->isEnabled())
                m_playBtn->animateClick();

            break;
        }

        case(Qt::Key_PageUp):
        {
            if (m_prevBtn->isEnabled())
                m_prevBtn->animateClick();

            break;
        }

        case(Qt::Key_PageDown):
        {
            if (m_nextBtn->isEnabled())
                m_nextBtn->animateClick();

            break;
        }

        case(Qt::Key_Escape):
        {
            if (m_stopBtn->isEnabled())
                m_stopBtn->animateClick();

            break;
        }

        default:
            break;
    }

    event->accept();
}

} // namespace KIPISlideShowPlugin
