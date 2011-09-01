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

            CropWidgetItem(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
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
