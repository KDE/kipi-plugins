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

#ifndef PAUSETIMER_H
#define PAUSETIMER_H

#include <qtimer.h>

extern "C"
{
#include <time.h>
}

namespace KIPISlideShowPlugin
{

/**
 * This Timer can be interrupted to pause a slideshow
 */
class PauseTimer : public QTimer
{
public:
    PauseTimer(QObject *parent=0, const char *name=0);
    
    int     start(int msec, bool sshot=FALSE);
    void    stop();
    bool    pause();
    bool    isPaused();
    
private:
    time_t  m_startTime;
    int     m_msecRest;
    int     m_msec;
    bool    m_paused;
};

}  // NameSpace KIPISlideShowPlugin

#endif /* PAUSETIMER_H */
