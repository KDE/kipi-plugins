//////////////////////////////////////////////////////////////////////////////
//
//    BATCHPROCESSIMAGESITEM.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for Qt

#include <qpainter.h>
#include <qlistview.h>

// Include files for KDE

#include <klocale.h>

// Local includes

#include "batchprocessimagesitem.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

BatchProcessImagesItem::BatchProcessImagesItem(QListView * parent, QString const & pathSrc,
                        QString const & nameSrc, QString const & nameDest, QString const & result)
                      : KListViewItem( parent, "", nameSrc, nameDest, result),
                        _pathSrc(pathSrc), _nameSrc(nameSrc), _nameDest(nameDest), _result(result), _overwrote( false )
{
    setText(0, pathSrc.section('/', -2, -2));
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

BatchProcessImagesItem::~BatchProcessImagesItem()
{
}


////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

QString BatchProcessImagesItem::pathSrc()                   { return _pathSrc;    }
QString BatchProcessImagesItem::nameSrc()                   { return _nameSrc;    }
QString BatchProcessImagesItem::nameDest()                  { return _nameDest;   }
QString BatchProcessImagesItem::result()                    { return _result;     }
QString BatchProcessImagesItem::error()                     { return _error;      }
QString BatchProcessImagesItem::outputMess()                { return _outputMess; }

void BatchProcessImagesItem::changeResult(QString text)     { setText(3, text); }
void BatchProcessImagesItem::changeError(QString text)      { _error = text; }
void BatchProcessImagesItem::changeNameDest(QString text)   { _nameDest = text; setText(2, _nameDest); }
void BatchProcessImagesItem::changeOutputMess(QString text) { _outputMess.append(text); }

void BatchProcessImagesItem::paintCell (QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    QColorGroup _cg( cg );

    if (text(3) != i18n("OK") && text(3) != "" )
       {
       _cg.setColor( QColorGroup::Text, Qt::red );
       KListViewItem::paintCell( p, _cg, column, width, alignment );
       return;
       }
    if (text(3) == i18n("OK") )
       {
       _cg.setColor( QColorGroup::Text, Qt::darkGreen );
       KListViewItem::paintCell( p, _cg, column, width, alignment );
       return;
       }

    KListViewItem::paintCell( p, cg, column, width, alignment );
}

bool BatchProcessImagesItem::overWrote()
{
    return _overwrote;
}

void BatchProcessImagesItem::setDidOverWrite( bool b )
{
    _overwrote = b;
}

}  // NameSpace KIPIBatchProcessImagesPlugin
