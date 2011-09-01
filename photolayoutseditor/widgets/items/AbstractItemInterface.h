#ifndef ABSTRACTITEMINTERFACE_H
#define ABSTRACTITEMINTERFACE_H

#include <QObject>
#include <QGraphicsItem>

namespace KIPIPhotoLayoutsEditor
{
    class Scene;
    class ScenePrivate;

    class AbstractItemInterface : public QObject, public QGraphicsItem
    {
        public:

            AbstractItemInterface(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);

        protected:

            virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
            virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
            virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
            virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

        friend class Scene;
        friend class ScenePrivate;
    };
}

#endif // ABSTRACTITEMINTERFACE_H
