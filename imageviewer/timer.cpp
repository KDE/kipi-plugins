/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "timer.h"

// KDE includes

#include <kdebug.h>

namespace KIPIViewerPlugin
{

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::start()
{
    m_timer.start();
    m_meantime = 0;
}

void Timer::at(const QString& s)
{
    m_meantime = m_timer.elapsed()-m_meantime;
    kDebug() << "stopwatch:" << s << ": " << m_meantime << " ms    overall: " << m_timer.elapsed() << " ms";
}

} // namespace KIPIViewerPlugin
