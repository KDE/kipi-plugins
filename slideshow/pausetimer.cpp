/* ============================================================
 * File  : pausetimer.h
 * Author: Jörn Ahrens <kde@jokele.de>
 * Date  : 2004-08-14
 * Description : PauseTimer impelements a QTimer derrived timer
 *               which can be paused.
 * 
 * Copyright 2004 by Jörn Ahrens

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#include "pausetimer.h"

extern "C"
{
#include <time.h>
}

namespace KIPISlideShowPlugin
{

PauseTimer::PauseTimer(QObject *parent, const char *name) :
    QTimer(parent, name)
{
    m_startTime = 0;
    m_msecRest = 0;
    m_paused = false;
}

int PauseTimer::start(int msec, bool sshot)
{
    m_startTime = time(0);
    m_msec = msec;
    return QTimer::start(msec, sshot);
}

bool PauseTimer::pause()
{
    if(!m_paused)
    {
        m_msecRest = m_msec - time(0) - m_startTime;
        stop();
    }
    else
    {
        if(m_msecRest < 0)
            start(0, true);
        else
            start(m_msecRest, true);
    }
    
    m_paused = !m_paused;
    return m_paused;
}

void PauseTimer::stop()
{
    m_startTime = 0;
    m_msecRest = 0;
    m_paused = false;    
    QTimer::stop();
}

bool PauseTimer::isPaused()
{
    return m_paused;
}

}  // NameSpace KIPISlideShowPlugin

