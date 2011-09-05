#ifndef IMAGELOADINGTHREAD_H
#define IMAGELOADINGTHREAD_H

#include <QThread>

namespace KIPIPhotoLayoutsEditor
{
    class ImageLoadingThread : public QThread
    {
            Q_OBJECT

        public:

            explicit ImageLoadingThread(QObject * parent = 0);
            virtual void run();

        signals:

            void loadingStarted(int bytesCount);
            void byteLoaded(int byte);
            void loadingFinished();

        public slots:

            void emitLoadingStarted();
            void emitByteLoaded(int byte);
            void emitLoadingFinished();

    };
}

#endif // IMAGELOADINGTHREAD_H
