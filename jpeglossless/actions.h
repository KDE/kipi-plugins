/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-12-05
 * Description : JPEGLossLess plugin action descriptions
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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
    FlipVertical = 1
};

class EventData
{

public:

    EventData() 
    {
        starting = false;
        success  = false;
    }

    bool    starting;
    bool    success;

    QString fileName;
    QString errString;

    Action  action;
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* ACTIONS_H */
