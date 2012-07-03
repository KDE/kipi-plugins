/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-07-01
 * @brief  convert images to ppm format 
 *
 * @author Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QObject>

// Local includes

#include "processimage.h"

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{

class ActionThread : public QObject
{
    Q_OBJECT

public:

    ActionThread();
    ~ActionThread();

Q_SIGNALS:

    void signalProcessError(const QString& errMess);

private:

    MagickApi*     m_api;
    ProcessImage*  processImg;
};

} // namespace KIPIVideoSlideShowPlugin

#endif // ACTIONTHREAD_H