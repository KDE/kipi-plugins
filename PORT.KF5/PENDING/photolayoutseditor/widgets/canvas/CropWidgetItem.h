/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef CROPWIDGETITEM_H
#define CROPWIDGETITEM_H

#include "AbstractItemInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhoto;
    class CropWidgetItemPrivate;

    class CropWidgetItem : public AbstractItemInterface
    {
            Q_OBJECT

            CropWidgetItemPrivate * d;

        public:

            explicit CropWidgetItem(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
            virtual ~CropWidgetItem();

            virtual QRectF boundingRect() const;
            virtual QPainterPath opaqueArea() const;
            virtual QPainterPath shape() const;
            virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

            virtual void keyPressEvent(QKeyEvent * event);
            virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

            void setItems(const QList<AbstractPhoto*> & items);

        signals:

            void cropShapeSelected(const QPainterPath & shape);
            void cancelCrop();

        private slots:

            void updateShapes();

        friend class CropWidgetItemPrivate;
    };
}

#endif // CROPWIDGETITEM_H
