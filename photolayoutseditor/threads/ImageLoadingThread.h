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

            void imageLoaded(const KUrl & url, const QImage & image);

        public slots:

            void setMaximumProgress(double limit);
            void setImageUrl(const KUrl & url);
            void setImagesUrls(const KUrl::List & urls);

        private:

            void loadRaw(const KUrl & url);
            void loadImage(const KUrl & url);

            class ImageLoadingThreadPrivate;
            friend class ImageLoadingThreadPrivate;
            ImageLoadingThreadPrivate * d;
    };
}

#endif // IMAGELOADINGTHREAD_H
