/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ABSTRACTLISTTOOL_P_H
#define ABSTRACTLISTTOOL_P_H

#include <QWidget>
#include <QListView>
#include <kpushbutton.h>

namespace KIPIPhotoLayoutsEditor
{
    class AbstractItemsListViewTool;

    class AbstractListToolViewDelegate : public QWidget
    {
            KPushButton * m_acceptButton;
            Q_OBJECT
        public:
            AbstractListToolViewDelegate(AbstractItemsListViewTool * parent = 0);
        signals:
            void editorClosed();
            void editorAccepted();
            void itemSelected(const QString & selectedItem);
        protected slots:
            void emitEditorClosed()
            {
                emit editorClosed();
            }
            void emitEditorAccepted()
            {
                emit editorAccepted();
            }
            void emitItemSelected(const QString & selectedItem)
            {
                m_acceptButton->setEnabled(!selectedItem.isEmpty());
                emit itemSelected(selectedItem);
            }
    };

    class AbstractListToolView : public QListView
    {
            Q_OBJECT
        public:
            AbstractListToolView(QWidget * parent = 0) :
                QListView(parent)
            {
                this->setSelectionMode(QAbstractItemView::SingleSelection);
                this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
            }
            QModelIndex selectedIndex() const
            {
                QModelIndexList indexes = selectedIndexes();
                if (indexes.count() == 1)
                    return indexes.at(0);
                return QModelIndex();
            }
        signals:
            void selectedIndex(const QModelIndex & index);
        protected:
            virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & /*deselected*/)
            {
                QModelIndexList indexes = selected.indexes();
                if (indexes.count())
                {
                    QModelIndex index = indexes.at(0);
                    if (index.isValid())
                    {
                        emit selectedIndex(index);
                        return;
                    }
                }
                emit selectedIndex(QModelIndex());
            }

        friend class AbstractItemsListViewTool;
    };
}

#endif // ABSTRACTLISTTOOL_P_H
