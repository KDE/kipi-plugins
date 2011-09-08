#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

#include "AbstractTool.h"

namespace KIPIPhotoLayoutsEditor
{
    class ZoomTool : public AbstractTool
    {
            Q_OBJECT

        public:

            ZoomTool(Scene * scene, QWidget * parent = 0);
            ~ZoomTool();

        public slots:

            void zoom(const QPointF & point);

        protected:

            virtual void sceneChange();
            virtual void sceneChanged();

        private:

            class ZoomToolPrivate;
            ZoomToolPrivate * d;
            friend class ZoomToolPrivate;
    };
}

#endif // ZOOMTOOL_H
