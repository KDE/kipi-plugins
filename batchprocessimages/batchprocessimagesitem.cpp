/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <qpainter.h>
#include <q3listview.h>

// Include files for KDE

#include <klocale.h>

// Local includes

#include "batchprocessimagesitem.h"

namespace KIPIBatchProcessImagesPlugin
{

BatchProcessImagesItem::BatchProcessImagesItem(Q3ListView * parent, QString const & pathSrc,
                        QString const & nameSrc, QString const & nameDest, QString const & result)
                      : K3ListViewItem( parent, "", nameSrc, nameDest, result),
                        _pathSrc(pathSrc), _nameSrc(nameSrc), _nameDest(nameDest), _result(result),
                        _overwrote( false ),
                        _reverseSort( false )
{
    setText(0, pathSrc.section('/', -2, -2));
}

BatchProcessImagesItem::~BatchProcessImagesItem()
{
}

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

    if (text(3) != i18n("OK") && !text(3).isEmpty() )
       {
       _cg.setColor( QColorGroup::Text, Qt::red );
       K3ListViewItem::paintCell( p, _cg, column, width, alignment );
       return;
       }
    if (text(3) == i18n("OK") )
       {
       _cg.setColor( QColorGroup::Text, Qt::darkGreen );
       K3ListViewItem::paintCell( p, _cg, column, width, alignment );
       return;
       }

    K3ListViewItem::paintCell( p, cg, column, width, alignment );
}

bool BatchProcessImagesItem::overWrote()
{
    return _overwrote;
}

void BatchProcessImagesItem::setDidOverWrite( bool b )
{
    _overwrote = b;
}

void BatchProcessImagesItem::setKey(const QString& val, bool reverseSort)
{
    _key = val;
    _reverseSort = reverseSort;
}

QString BatchProcessImagesItem::key(int column, bool ) const
{
    if (_key.isNull())
        return text(column);

    return _key;
}

int BatchProcessImagesItem::compare(Q3ListViewItem * i, int col, bool ascending) const
{
    int weight = _reverseSort ? -1 : 1;
    return weight * key(col, ascending).localeAwareCompare(i->key( col, ascending));
}

}  // NameSpace KIPIBatchProcessImagesPlugin
