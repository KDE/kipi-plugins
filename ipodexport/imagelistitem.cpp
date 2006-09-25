//////////////////////////////////////////////////////////////////////////////
//
//    ImageListItem.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
//              (C) 2006 Seb Ruiz <me@sebruiz.net>
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

// Include files for Qt

#include <qpainter.h>
#include <qlistview.h>

// Include files for KDE

#include <klocale.h>

// Local includes

#include "imagelistitem.h"

using namespace IpodExport;

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ImageListItem::ImageListItem( QListView * parent, QString const &pathSrc,
                              QString const & name, QString const & result )
        : KListViewItem( parent, ""/*set below*/, name, result )
        , m_pathSrc( pathSrc )
        , m_name( name )
        , m_result( result )
        , m_overwrote( false )
        , m_reverseSort( false )
{
    setText( 0, pathSrc.section('/', -2, -2) );
}


////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

QString ImageListItem::pathSrc()                  { return m_pathSrc;    }
QString ImageListItem::name()                     { return m_name;       }
QString ImageListItem::result()                   { return m_result;     }
QString ImageListItem::error()                    { return m_error;      }
QString ImageListItem::outputMess()               { return m_outputMsg;  }

void ImageListItem::changeResult(QString text)    { setText(3, text); }
void ImageListItem::changeError(QString text)     { m_error = text; }
void ImageListItem::changeOutputMsg(QString text) { m_outputMsg.append(text); }

void ImageListItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment )
{
    QColorGroup _cg( cg );

    if (text(3) != i18n("OK") && !text(3).isEmpty() )
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

void ImageListItem::setKey( const QString& val, bool reverseSort )
{
    m_key = val;
    m_reverseSort = reverseSort;
}

QString ImageListItem::key( int column, bool ) const
{
    if( m_key.isNull())
        return text( column );

    return m_key;
}

int ImageListItem::compare( QListViewItem * i, int col, bool ascending ) const
{
    int weight = m_reverseSort ? -1 : 1;
    return weight * key(col, ascending).localeAwareCompare(i->key( col, ascending));
}
