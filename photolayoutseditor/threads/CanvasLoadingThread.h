#ifndef CANVASLOADINGTHREAD_H
#define CANVASLOADINGTHREAD_H

#include <QThread>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasLoadingThread : public QThread
    {
            Q_OBJECT

        public:

            explicit CanvasLoadingThread(QObject *parent = 0);
            virtual void run();

        private:

            class CanvasLoadingThreadPrivate;
            CanvasLoadingThreadPrivate * d;
            friend class CanvasLoadingThreadPrivate;
    };
}

#endif // CANVASLOADINGTHREAD_H
