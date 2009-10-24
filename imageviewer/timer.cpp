/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using
 *               an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
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

using namespace KIPIviewer;

Timer::Timer()
{
}

void Timer::start()
{
    timer.start();
    meantime=0;
}

void Timer::at(QString s)
{
    meantime=timer.elapsed()-meantime;
    kDebug() << "stopwatch:"<< s << ": " << meantime << " ms    overall: " << timer.elapsed() << " ms";
}

Timer::~Timer()
{
}
