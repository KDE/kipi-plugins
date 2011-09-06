#ifndef IMAGELOADINGTHREAD_H
#define IMAGELOADINGTHREAD_H

#include <QThread>
#include <QImage>

#include <kurl.h>

namespace KIPIPhotoLayoutsEditor
{
    class ImageLoadingThread : public QThread
    {
            Q_OBJECT

        public:

            explicit ImageLoadingThread(QObject * parent = 0);
            ~ImageLoadingThread();
            virtual void run();

        signals:

            void loadingStarted(int length);
            void loadingProgress(int step);
            void loadingFinished();
            void imageLoaded(const KUrl & url, const QImage & image);

        public slots:

            void slotReadImage(const KUrl & url);
            void slotReadImages(const KUrl::List & urls);
            void loaded() {}

        private:

            class ImageLoadingThreadPrivate;
            friend class ImageLoadingThreadPrivate;
            ImageLoadingThreadPrivate * d;
    };
}

#endif // IMAGELOADINGTHREAD_H
