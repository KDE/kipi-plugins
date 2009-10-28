/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "batchprocessimagesitem.h"

// Qt includes

#include <QPainter>
#include <QTreeWidget>

// KDE includes

#include <klocale.h>

namespace KIPIBatchProcessImagesPlugin
{

int BatchProcessImagesItem::columnOfSortKey()
{
    return 4;
}

BatchProcessImagesItem::BatchProcessImagesItem(QTreeWidget* parent, QString const& pathSrc,
                                               QString const& nameSrc, QString const& nameDest, QString const& result)
                      : QTreeWidgetItem(parent),
                        _overwrote(false),
                        _pathSrc(pathSrc), 
                        _nameSrc(nameSrc), 
                        _nameDest(nameDest), 
                        _result(result)
{
    setText(0, pathSrc.section('/', -2, -2));
    setText(1, nameSrc);
    setText(2, nameDest);
    setText(3, result);
}

BatchProcessImagesItem::~BatchProcessImagesItem()
{
}

QString BatchProcessImagesItem::pathSrc()
{
    return _pathSrc;
}
QString BatchProcessImagesItem::nameSrc()
{
    return _nameSrc;
}
QString BatchProcessImagesItem::nameDest()
{
    return _nameDest;
}
QString BatchProcessImagesItem::result()
{
    return _result;
}
QString BatchProcessImagesItem::error()
{
    return _error;
}
QString BatchProcessImagesItem::outputMess()
{
    return _outputMess;
}

QString BatchProcessImagesItem::sortKey()
{
    return _sortKey;
}

void BatchProcessImagesItem::changeResult(QString text)
{
    setText(3, text);
}
void BatchProcessImagesItem::changeError(QString text)
{
    _error = text;
}
void BatchProcessImagesItem::changeNameDest(QString text)
{
    _nameDest = text; setText(2, _nameDest);
}
void BatchProcessImagesItem::changeOutputMess(QString text)
{
    _outputMess.append(text);
}

void BatchProcessImagesItem::changeSortKey(QString text)
{
    _sortKey = text;
    setText(columnOfSortKey(), text);
}

bool BatchProcessImagesItem::overWrote()
{
    return _overwrote;
}

void BatchProcessImagesItem::setDidOverWrite(bool b)
{
    _overwrote = b;
}

/* FIXME
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
*/

}  // namespace KIPIBatchProcessImagesPlugin
