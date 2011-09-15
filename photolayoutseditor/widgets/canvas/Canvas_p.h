/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef CANVAS_P_H
#define CANVAS_P_H

#include "CanvasSize.h"

#include <QMap>
#include <QProgressBar>

namespace KIPIPhotoLayoutsEditor
{
    class Canvas;
    class CanvasSavingThread;
    class CanvasPrivate
    {
        CanvasSize m_size;

        QMap<QObject*,QProgressBar*> progressMap;

        friend class Canvas;
        friend class CanvasSavingThread;
    };
};

#endif // CANVAS_P_H
