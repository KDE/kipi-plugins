#ifndef MOUSEPRESSLISTENER_H
#define MOUSEPRESSLISTENER_H

#include <QObject>
#include <QPointF>

namespace KIPIPhotoLayoutsEditor
{
    class MousePressListener : public QObject
    {
            Q_OBJECT

        public slots:

            void mousePress(const QPointF & scenePos)
            {
                emit mousePressed(scenePos);
            }

        signals:

            void mousePressed(const QPointF & scenePos);
    };
}

#endif // MOUSEPRESSLISTENER_H
