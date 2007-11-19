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

    void setKey(const QString& val, bool reverseSort);
    QString key(int column, bool ascending) const;
    int compare (QListViewItem * i, int col, bool ascending) const;

private:
    QString _pathSrc;
    QString _nameSrc;
    QString _nameDest;
    QString _result;
    QString _error;
    QString _outputMess;
    bool    _overwrote;
    QString _key;
    bool    _reverseSort;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // BATCHPROCESSIMAGESITEM_H
