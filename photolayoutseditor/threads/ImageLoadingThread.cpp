#include "ImageLoadingThread.h"
#include "ProgressEvent.h"
#include "photolayoutseditor.h"

#include <QCoreApplication>
#include <QSemaphore>
#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QDebug>

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

class ImageLoadingThread::ImageLoadingThreadPrivate
{
    ImageLoadingThreadPrivate() :
        m_sem(1),
        m_size(0),
        m_max_progress(100)
    {}

    KUrl::List m_urls;
    QSemaphore m_sem;
    qint64 m_size;
    qint64 m_loaded_bytes;
    double m_max_progress;

    friend class ImageLoadingThread;
};

ImageLoadingThread::ImageLoadingThread(QObject * parent) :
    QThread(parent),
    d(new ImageLoadingThreadPrivate)
{
}

ImageLoadingThread::~ImageLoadingThread()
{
    delete d;
}

void ImageLoadingThread::run()
{
    KUrl::List urls = d->m_urls;

    // Calculating reading progress
    d->m_loaded_bytes = d->m_size = 0;
    foreach (KUrl url, urls)
    {
        QFile f(url.path());
        f.open(QIODevice::ReadOnly);
        if (f.isReadable())
        {
            d->m_sem.acquire();
            d->m_size += f.size();
            d->m_sem.release();
        }
        f.close();
    }

    if (!d->m_size)
        goto finish_thread;

    // Reading
    foreach (KUrl url, urls)
    {
        ProgressEvent * startEvent = new ProgressEvent();
        startEvent->setData(ProgressEvent::Init, 0);
        QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), startEvent);
        QCoreApplication::processEvents();

        ProgressEvent * loadingImageActionEvent = new ProgressEvent();
        loadingImageActionEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Loading ").append(url.fileName()) ));
        QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), loadingImageActionEvent);
        QCoreApplication::processEvents();

        QFile f(url.path());
        f.open(QIODevice::ReadOnly);
        QByteArray ba;
        QBuffer bf(&ba);
        bf.open(QIODevice::WriteOnly);
        QByteArray temp;
        int s = f.size() / 10;
        s = s < 1000 ? 1000 : s;
        do
        {
            temp = f.read(s);
            d->m_loaded_bytes += temp.size();
            bf.write(temp.data(), temp.size());
            this->yieldCurrentThread();
            ProgressEvent * event = new ProgressEvent();
            event->setData(ProgressEvent::ProgressUpdate, (d->m_loaded_bytes * d->m_max_progress) / (d->m_size * 1.4));
            QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), event);
            QCoreApplication::processEvents();
        }
        while (temp.size() == s);
        f.close();
        bf.close();

        ProgressEvent * buildImageEvent = new ProgressEvent();
        buildImageEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Building image") ));
        QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), buildImageEvent);
        QCoreApplication::processEvents();

        QImage img = QImage::fromData(ba);
        emit imageLoaded(url, img);

        ProgressEvent * event = new ProgressEvent();
        event->setData(ProgressEvent::ProgressUpdate, (d->m_loaded_bytes * d->m_max_progress) / d->m_size);
        QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), event);
        QCoreApplication::processEvents();
    }

finish_thread:
    this->exit(0);
    this->deleteLater();
}

void ImageLoadingThread::setMaximumProgress(double limit)
{
    if (limit > 100)
        limit = 100;
    d->m_max_progress = limit;
}

void ImageLoadingThread::setImageUrl(const KUrl & url)
{
    d->m_sem.acquire();
    d->m_urls.clear();
    d->m_urls.append(url);
    d->m_sem.release();
}

void ImageLoadingThread::setImagesUrls(const KUrl::List & urls)
{
    d->m_sem.acquire();
    d->m_urls = urls;
    d->m_sem.release();
}
