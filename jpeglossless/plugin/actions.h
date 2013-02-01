/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-12-05
 * Description : JPEGLossLess plugin action descriptions
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef ACTIONS_H
#define ACTIONS_H

namespace KIPIJPEGLossLessPlugin
{

enum Action
{
    Rotate = 0,
    Flip,
    GrayScale
};

enum RotateAction
{
    Rot90 = 0,
    Rot180,
    Rot270,
    Rot0
};

enum FlipAction
{
    FlipHorizontal = 0,
    FlipVertical   = 1
};

}  // namespace KIPIJPEGLossLessPlugin

#endif /* ACTIONS_H */
