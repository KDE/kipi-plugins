//////////////////////////////////////////////////////////////////////////////
//
//    BATCHPROCESSIMAGESITEM.H
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

#ifndef BATCHPROCESSIMAGESITEM_H
#define BATCHPROCESSIMAGESITEM_H

// Include files for Qt

#include <qstring.h>
#include <qobject.h>

// Include files for KDE

#include <klistview.h>

class QPainter;
class QListView;

namespace IpodExport
{

class ImageListItem : public KListViewItem
{
public:

    ImageListItem( QListView * parent, QString const & pathSrc,
                   QString const & name, QString const & result);

    ~ImageListItem() { }

    QString pathSrc();
    QString name();
    QString result();
    QString error();
    QString outputMess();

    bool overWrote() const               { return m_overwrote; }
    void setDidOverWrite( const bool b ) { m_overwrote = b;    }

    void changeResult( QString text );
    void changeError( QString text );
    void changeNameDest( QString text );
    void changeOutputMsg( QString text );

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );

    void setKey( const QString& val, bool reverseSort );
    QString key( int column, bool ascending ) const;
    int compare( QListViewItem * i, int col, bool ascending ) const;

private:
    QString m_pathSrc;
    QString m_name;
    QString m_result;
    QString m_error;
    QString m_outputMsg;
    bool    m_overwrote;
    QString m_key;
    bool    m_reverseSort;
};

}

#endif  // BATCHPROCESSIMAGESITEM_H
