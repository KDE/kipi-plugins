/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef GLOBAL_H
#define GLOBAL_H

#define Q_DELETE(ptr)   if(ptr) { delete ptr; ptr = 0; }

#include <QUndoCommand>
#include <QPrinter>
#include <QSizeF>
#include <QMap>
#include <QDomDocument>
#include <QPainterPath>

namespace KIPIPhotoLayoutsEditor
{
    extern QString name();
    extern QString uri();
    extern QString templateUri();
    extern void PLE_PostUndoCommand(QUndoCommand * command);
    extern QDomDocument pathToSvg(const QPainterPath & path);
    extern QPainterPath pathFromSvg(const QDomElement & element);
}

#endif // GLOBAL_H
