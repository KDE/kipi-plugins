/* ============================================================
 * File  : imageflip.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-10-14
 * Description : batch image flip
 * 
 * Copyright 2003 by Gilles Caulier

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

#ifndef IMAGEFLIP_H
#define IMAGEFLIP_H

#include "actions.h"

class QString;
class QImage;

namespace KIPIJPEGLossLessPlugin
{

bool flip(const QString& src, FlipAction action,
         const QString& TmpFolder, QString& err);
bool flipJPEG(const QString& src, const QString& dest,
              FlipAction action, QString& err);
bool flipQImage(const QString& src, const QString& dest,
                FlipAction action, QString& err);

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* IMAGEFLIP_H */
