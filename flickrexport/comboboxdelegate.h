/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-07-05
 * Description : A combobox delegate to display in image lists.
 *
 * Copyright (C) 2009 by Pieter Edelman <pieter dot edelman at gmx dot net>
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

#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

// Qt includes

#include <QAbstractItemDelegate>
#include <QAbstractItemModel>
#include <QMap>
#include <QModelIndex>
#include <QPainter>
#include <QSize>
#include <QString>
#include <QStyleOptionViewItem>
#include <QWidget>

// Local includes

#include "kpimageslist.h"

using namespace KIPIPlugins;

namespace KIPIFlickrExportPlugin
{

class ComboBoxDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:

    ComboBoxDelegate(KPImagesList* const, QMap<int, QString>);

    /* Whenever an element needs to be edited, this method should be called.
     * It's actually a hack to prevent the item text shining through whenever
     * editing occurs. */
    void startEditing(QTreeWidgetItem*, int);

    /* Overloaded functions to provide the delegate functionality. */
    void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;
    void setEditorData(QWidget*, const QModelIndex&) const;
    void setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const;

private Q_SLOTS:

    void commitAndCloseEditor(int);
    void slotResetEditedState(QObject*);

private:

    KPImagesList*            m_parent;
    QMap<int, QString>       m_items;

    /* The row in the view that is currently being edited. Should be -1 to
     * indicate that no row is edited. */
    int                      m_rowEdited;

    QSize                    m_size;
};

} // namespace KIPIFlickrExportPlugin

#endif /* COMBOBOXDELEGATE_H */
