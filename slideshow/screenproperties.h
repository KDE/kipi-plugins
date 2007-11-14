/* ============================================================
 * File  : screenproperties.h
 * Author: Valerio Fuoglio <valerio.fuoglio@gmail.com>
 * Date  : 2007-11-14
 * Description : 
 * 
 * Copyright 2007 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
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

/***************************************************************************
 *   Parts of this code are based on smoothslidesaver by Carsten Weinhold  *
 *   <carsten.weinhold@gmx.de>                                             *
 **************************************************************************/

#ifndef SCREEN_H
#define SCREEN_H

namespace KIPISlideShowPlugin
{

  class ScreenProperties {
    public:
        ScreenProperties(QWidget *mainWidget);
        virtual ~ScreenProperties() { };
    
        virtual unsigned suggestFrameRate();
        virtual bool     enableVSync();
    
    protected:
        unsigned activeScreen;
  };

}  // NameSpace KIPISlideShowPlugin
#endif
