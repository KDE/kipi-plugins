/* ============================================================
 * File  : actions.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-12-05
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
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

namespace FindImages
{

enum Action
{
    Similar = 0,
    Exact,
    Matrix,
    Progress
};
    


class EventData
{
public:
    EventData() {
        starting = false;
        success  = false;
    }
    
    QString fileName;
    QString errString;
    int     total;
    bool    starting;
    bool    success;
    Action  action;
};


}

#endif /* ACTIONS_H */
