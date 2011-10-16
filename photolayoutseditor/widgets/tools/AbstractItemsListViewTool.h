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

#ifndef ABSTRACTLISTTOOL_H
#define ABSTRACTLISTTOOL_H

#include "AbstractItemsTool.h"
#include "AbstractMovableModel.h"

namespace KIPIPhotoLayoutsEditor
{
    class AbstractItemsListViewToolPrivate;

    class AbstractListToolView;
    class AbstractListToolViewDelegate;

    class AbstractItemsListViewTool : public AbstractItemsTool
    {
            Q_OBJECT

        public:
            explicit AbstractItemsListViewTool(const QString & toolsName, Scene * scene, Canvas::SelectionMode selectionMode, QWidget * parent = 0);
            virtual ~AbstractItemsListViewTool();
            virtual void currentItemAboutToBeChanged();
            virtual void currentItemChanged();
            virtual void positionAboutToBeChanged(){} // Unused
            virtual void positionChanged(){} // Unused
            virtual QStringList options() const = 0;
            virtual QObject * createItem(const QString & name) = 0;

        protected slots:

            virtual AbstractMovableModel * model() = 0;
            void viewCurrentEditor(const QModelIndex & index);
            void viewCurrentEditor(QObject * object);
            virtual QWidget * createEditor(QObject * item, bool createCommands = true) = 0;
            void createChooser();
            void closeChooser();
            void removeSelected();
            void moveSelectedDown();
            void moveSelectedUp();

        private:
            void closeEditor();
            AbstractItemsListViewToolPrivate * d;
    };
}

#endif // ABSTRACTLISTTOOL_H
