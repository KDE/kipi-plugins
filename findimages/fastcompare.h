//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEIMAGES.CPP
//
//    Copyright (C) 2001 Richard Groult <rgroult at jalix.org> (from ShowImg project)
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
//    Copyright (C) 2004 Richard Groult <rgroult at jalix.org>
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

#ifndef FASTCOMPARE_H
#define FASTCOMPARE_H

class QStringList;
class QObject;
#include <qdict.h>
#include <qptrvector.h>
#include <qfile.h>

namespace KIPIFindDupplicateImagesPlugin
{

class FastCompare
{
public:
    FastCompare( QObject* parent );
    QDict < QPtrVector < QFile > > doFastCompare( const QStringList& filesList, int* total );

protected:
    bool equals(QFile*, QFile*); // Return true if the 2 files are the sames.

private:
    QObject* m_parent;
};

}

#endif /* FASTCOMPARE_H */

