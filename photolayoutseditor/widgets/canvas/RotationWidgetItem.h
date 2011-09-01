#ifndef QGRAPHICSSELECTIONITEM_P_H
#define QGRAPHICSSELECTIONITEM_P_H

#include <qmath.h>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "AbstractPhoto.h"

namespace KIPIPhotoLayoutsEditor
{
    class RotationWidgetItemPrivate;
    class RotateItemCommand;

    class RotationWidgetItem : public AbstractItemInterface
    {
            Q_OBJECT

            RotationWidgetItemPrivate * d;

        public:

            RotationWidgetItem(const QList<AbstractPhoto*> & items, QGraphicsItem * parent = 0);
            virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
            virtual QPainterPath shape() const;
            virtual QPainterPath opaqueArea() const;
            virtual QRectF boundingRect() const;
            void initRotation(const QPainterPath & path, const QPointF & rotationPoint);
            void reset();
            qreal angle() const;
            QPointF rotationPoint() const;
            bool isRotated() const;

        protected:

            virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
            virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
            virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
            virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);

            void setItems(const QList<AbstractPhoto*> & items);

        Q_SIGNALS:

            void rotationChanged(const QPointF & point, qreal angle);
            void rotationFinished(const QPointF & point, qreal angle);

        friend class QGraphicsEditingWidget;
        friend class RotateItemCommand;
    };

}

#endif // QGRAPHICSSELECTIONITEM_P_H
