//////////////////////////////////////////////////////////////////////////////
//
//    BATCHPROCESSIMAGESITEM.H
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

#ifndef BATCHPROCESSIMAGESITEM_H
#define BATCHPROCESSIMAGESITEM_H

// Include files for Qt

#include <qstring.h>
#include <qobject.h>

// Include files for KDE

#include <klistview.h>

class QPainter;
class QListView;

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem : public KListViewItem
{
public:
    BatchProcessImagesItem(QListView * parent, QString const & pathSrc, QString const & nameSrc,
                           QString const & nameDest, QString const & result);

    ~BatchProcessImagesItem();

    QString pathSrc();
    QString nameSrc();
    QString nameDest();
    QString result();
    QString error();
    QString outputMess();
    bool overWrote();
    void setDidOverWrite( bool b );

    void changeResult(QString text);
    void changeError(QString text);
    void changeNameDest(QString text);
    void changeOutputMess(QString text);

    void paintCell (QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

private:
    QString _pathSrc;
    QString _nameSrc;
    QString _nameDest;
    QString _result;
    QString _error;
    QString _outputMess;
    bool    _overwrote;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // BATCHPROCESSIMAGESITEM_H
