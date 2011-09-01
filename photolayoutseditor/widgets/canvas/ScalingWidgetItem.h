#ifndef SCALINGWIDGETITEM_H
#define SCALINGWIDGETITEM_H

#include "AbstractItemInterface.h"

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhoto;
    class ScalingWidgetItemPrivate;

    class MoveItemCommand;
    class ScaleItemCommand;

    class ScalingWidgetItem : public AbstractItemInterface
    {
            Q_OBJECT

            ScalingWidgetItemPrivate * d;

        public:

            ScalingWidgetItem(const QList<AbstractPhoto*> & items, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
            virtual ~ScalingWidgetItem();

            virtual QRectF boundingRect() const;
            virtual QPainterPath opaqueArea() const;
            virtual QPainterPath shape() const;
            virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

            virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

        private:

            void setScaleItems(const QList<AbstractPhoto*> & items);

        private slots:

            void updateShapes();

        friend class ScalingWidgetItemPrivate;

        friend class MoveItemCommand;
        friend class ScaleItemCommand;
    };
}

#endif // SCALINGWIDGETITEM_H
