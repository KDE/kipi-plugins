/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at gmail dot com>
 *         from digiKam project.
 * Date  : 2004-02-25
 * Description : a kipi plugin for e-mailing images
 * 
 * Copyright 2004-2005 by Gilles Caulier
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

namespace KIPISendimagesPlugin
{

enum Action
{
    Initialize = 0,
    Progress,
    Error,
    ResizeImages
};
    

class EventData
{
public:
    EventData() 
       {
       starting = false;
       success  = false;
       }
    
    QString fileName;
    QString albumName;
    QString errString;
    int     total;
    bool    starting;
    bool    success;
    Action  action;
};

}  // NameSpace KIPISendimagesPlugin

#endif  // ACTIONS_H 
