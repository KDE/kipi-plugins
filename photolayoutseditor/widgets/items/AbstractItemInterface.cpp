#include "AbstractItemInterface.h"

#include <QGraphicsScene>

using namespace KIPIPhotoLayoutsEditor;

AbstractItemInterface::AbstractItemInterface(QGraphicsItem * parent, QGraphicsScene * scene) :
    QObject(scene),
    QGraphicsItem(parent, scene)
{
}

void AbstractItemInterface::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
}

void AbstractItemInterface::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void AbstractItemInterface::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void AbstractItemInterface::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
}
