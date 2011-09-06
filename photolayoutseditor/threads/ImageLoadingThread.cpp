#include "ImageLoadingThread.h"
#include "ProgressEvent.h"
#include "photolayoutseditor.h"

#include <QCoreApplication>
#include <QImageReader>
#include <QSemaphore>
#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QDebug>

using namespace KIPIPhotoLayoutsEditor;

class ImageLoadingThread::ImageLoadingThreadPrivate
{
    ImageLoadingThreadPrivate() :
        m_sem(1),
        m_size(0)
    {}

    KUrl::List m_urls;
    QSemaphore m_sem;
    qint64 m_size;
    qint64 m_loaded_bytes;

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
    qDebug() << "Thread destroyed";
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

    ProgressEvent * startEvent = new ProgressEvent();
    startEvent->setData(ProgressEvent::Init, 1000);
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), startEvent);
    QCoreApplication::processEvents();

    if (!d->m_size)
        goto finish;

    // Reading
    foreach (KUrl url, urls)
    {
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
            event->setData(ProgressEvent::Update, (int)(d->m_loaded_bytes * 1000 / d->m_size));
            QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), event);
            QCoreApplication::processEvents();
        }
        while (temp.size() == s);
        f.close();
        bf.close();
        QImage img = QImage::fromData(ba);
        emit imageLoaded(url, img);
    }

finish:
    ProgressEvent * finishEvent = new ProgressEvent();
    finishEvent->setData(ProgressEvent::Finish, 0);
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), finishEvent);
    QCoreApplication::processEvents();

    this->exit(0);
    this->deleteLater();
}

void ImageLoadingThread::slotReadImage(const KUrl & url)
{
    d->m_sem.acquire();
    d->m_urls.clear();
    d->m_urls.append(url);
    d->m_sem.release();
}

void ImageLoadingThread::slotReadImages(const KUrl::List & urls)
{
    d->m_sem.acquire();
    d->m_urls = urls;
    d->m_sem.release();
}
