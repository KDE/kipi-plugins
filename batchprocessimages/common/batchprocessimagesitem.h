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

#ifndef BATCHPROCESSIMAGESITEM_H
#define BATCHPROCESSIMAGESITEM_H

// Qt includes

#include <QObject>
#include <QString>
#include <QTreeWidgetItem>

// KDE includes

class QPainter;

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem : public QTreeWidgetItem
{
public:

    static int columnOfSortKey();

    BatchProcessImagesItem(QTreeWidget * parent, QString const & pathSrc, QString const & nameSrc,
                           QString const & nameDest, QString const & result);

    ~BatchProcessImagesItem();

    QString pathSrc() const;
    QString nameSrc() const;
    QString nameDest() const;
    QString result() const;
    QString error() const;
    QString outputMess() const;
    QString sortKey() const;
    bool overWrote() const;
    void setDidOverWrite(bool b);

    void changeResult(const QString& text);
    void changeError(const QString& text);
    void changeNameDest(const QString& text);
    void changeOutputMess(const QString& text);
    void changeSortKey(const QString& text);

    //void paintCell (QPainter *p, const QColorGroup &cg, int column, int width, int alignment);

private:

    bool    _overwrote;

    QString _pathSrc;
    QString _nameSrc;
    QString _nameDest;
    QString _result;
    QString _error;
    QString _outputMess;
    QString _sortKey;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // BATCHPROCESSIMAGESITEM_H
