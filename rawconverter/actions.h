/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2006-12-09
 * Description : raw converter plugin action descriptions
 *
 * Copyright 2006-2007 by Gilles Caulier
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

namespace KIPIRawConverterPlugin
{

enum Action
{
    NONE = 0,
    IDENTIFY,
    PREVIEW,
    PROCESS
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

    QString filePath;
    QString destPath;
    QString message;

    QImage  image;

    Action  action;
};

}  // NameSpace KIPIRawConverterPlugin

#endif /* ACTIONS_H */
