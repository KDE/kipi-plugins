/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

QString BatchProcessImagesItem::pathSrc() const
{
    return _pathSrc;
}
QString BatchProcessImagesItem::nameSrc() const
{
    return _nameSrc;
}
QString BatchProcessImagesItem::nameDest() const
{
    return _nameDest;
}
QString BatchProcessImagesItem::result() const
{
    return _result;
}
QString BatchProcessImagesItem::error() const
{
    return _error;
}
QString BatchProcessImagesItem::outputMess() const
{
    return _outputMess;
}

QString BatchProcessImagesItem::sortKey() const
{
    return _sortKey;
}

void BatchProcessImagesItem::changeResult(const QString& text)
{
    setText(3, text);
}
void BatchProcessImagesItem::changeError(const QString& text)
{
    _error = text;
}
void BatchProcessImagesItem::changeNameDest(const QString& text)
{
    _nameDest = text; setText(2, _nameDest);
}
void BatchProcessImagesItem::changeOutputMess(const QString& text)
{
    _outputMess.append(text);
}

void BatchProcessImagesItem::changeSortKey(const QString& text)
{
    _sortKey = text;
    setText(columnOfSortKey(), text);
}

bool BatchProcessImagesItem::overWrote() const
{
    return _overwrote;
}

void BatchProcessImagesItem::setDidOverWrite(bool b)
{
    _overwrote = b;
}

}  // namespace KIPIBatchProcessImagesPlugin
