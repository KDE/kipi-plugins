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

#ifndef TIMER_H
#define TIMER_H

// Qt includes

#include <QDateTime>
#include <QString>

/**
 * @short convenience class for profiling
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.2
 */
namespace KIPIviewer
{

class Timer
{

public:

    Timer();
    ~Timer();
    void start();
    void at(QString s);

protected:

    QTime timer;
    int meantime;
};

} //namespace KIPIviewer

#endif // TIMER_H
