#ifndef CANVASSAVINGTHREAD_H
#define CANVASSAVINGTHREAD_H

#include <QThread>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasSavingThread : public QThread
    {
        public:

            explicit CanvasSavingThread(QObject * parent = 0);

        protected:

            virtual void run();

    };
}

#endif // CANVASSAVINGTHREAD_H
