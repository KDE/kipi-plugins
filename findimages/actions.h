//////////////////////////////////////////////////////////////////////////////
//
//    ACTIONS.H
//
//    Copyright (C) 2004 Richard Groult <rgroult at jalix.org>
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ACTIONS_H
#define ACTIONS_H

namespace KIPIFindDupplicateImagesPlugin
{

enum Action
{
    Similar = 0,
    Exact,
    Matrix,
    FastParsing,
    Progress
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
    QString errString;
    int     total;
    bool    starting;
    bool    success;
    Action  action;
};


}  // NameSpace KIPIFindDupplicateImagesPlugin

#endif // ACTIONS_H 
