//////////////////////////////////////////////////////////////////////////////
//
//    CHECKBINPROG.H
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at free.fr>
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

#ifndef CheckBinProg_included
#define CheckBinProg_included

#include <qobject.h>
#include <qstring.h>

class KConfig;

namespace KIPIMPEGEncoderPlugin
{

class CheckBinProg : public QObject
{
Q_OBJECT

public:
    CheckBinProg(QObject *parent=0);
    virtual ~CheckBinProg();
    int findExecutables( void );

private:
    KConfig* config;
    QString  ImageMagickPath;
    QString  MjpegToolsPath;
};

}  // NameSpace KIPIMPEGEncoderPlugin

#endif // CheckBinProg_included

