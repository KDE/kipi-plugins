#include "ImageLoadingThread.h"
#include "ProgressEvent.h"
#include "photolayoutseditor.h"

#include <QCoreApplication>
#include <QSemaphore>
#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QFileInfo>
#include <QDebug>

#include <klocalizedstring.h>
#include <libkdcraw/kdcraw.h>
#include <libkdcraw/rawdecodingsettings.h>

using namespace KIPIPhotoLayoutsEditor;

class ImageLoadingThread::ImageLoadingThreadPrivate
{
    ImageLoadingThreadPrivate() :
        m_sem(1),
        m_size(0),
        m_max_progress(1)
    {}

    KUrl::List m_urls;
    QSemaphore m_sem;
    qint64 m_size;
    qint64 m_loaded_bytes;
    double m_max_progress;

    friend class ImageLoadingThread;
};

class RAWLoader : public KDcrawIface::KDcraw
{
        double m_max_progress;
    public:
        RAWLoader() :
            m_max_progress(100)
        {}
        void setMaxDataProgress(double value)
        {
            m_max_progress = value;
        }
    protected:
        virtual void setWaitingDataProgress(double value)
        {
            ProgressEvent * event = new ProgressEvent();
            event->setData(ProgressEvent::ProgressUpdate, value * m_max_progress / 0.4);
            QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), event);
            QCoreApplication::processEvents();
        }
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

        QFileInfo fileInfo(url.path());
        QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
        QString ext = fileInfo.suffix().toUpper();

        if (rawFilesExt.toUpper().contains(ext))
            loadRaw(url);
        else
            loadImage(url);

        ProgressEvent * finishEvent = new ProgressEvent();
        finishEvent->setData(ProgressEvent::Finish, 1);
        QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), finishEvent);
        QCoreApplication::processEvents();
    }

finish_thread:
    this->exit(0);
    this->deleteLater();
}

void ImageLoadingThread::setMaximumProgress(double limit)
{
    if (limit > 1)
        limit = 1;
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

void ImageLoadingThread::loadRaw(const KUrl & url)
{
    ProgressEvent * loadingImageActionEvent = new ProgressEvent();
    loadingImageActionEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Loading ").append(url.fileName()) ));
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), loadingImageActionEvent);
    QCoreApplication::processEvents();

    RAWLoader loader;
    loader.setMaxDataProgress(d->m_max_progress * 0.7);
    KDcrawIface::RawDecodingSettings settings;
    QByteArray ba;
    int width;
    int height;
    int rgbamax;
    loader.decodeRAWImage(url.path(), settings, ba, width, height, rgbamax);

    ProgressEvent * buildImageEvent = new ProgressEvent();
    buildImageEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Decoding image") ));
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), buildImageEvent);
    QCoreApplication::processEvents();

    QImage img;
    uchar * image = new uchar[width*height*4];
    if (image)
    {
        uchar* dst   = image;
        uchar* src   = (uchar*)ba.data();

        for (int h = 0; h < height; ++h)
        {
            ProgressEvent * event = new ProgressEvent();
            event->setData(ProgressEvent::ProgressUpdate, d->m_max_progress * (0.7 + 0.3 * (((float)h)/((float)height))) );
            QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), event);
            QCoreApplication::processEvents();

            for (int w = 0; w < width; ++w)
            {
                // No need to adapt RGB components accordingly with rgbmax value because dcraw
                // always return rgbmax to 255 in 8 bits/color/pixels.

                dst[0] = src[2];    // Blue
                dst[1] = src[1];    // Green
                dst[2] = src[0];    // Red
                dst[3] = 0xFF;      // Alpha

                dst += 4;
                src += 3;
            }
        }

        img = QImage(width, height, QImage::Format_ARGB32);

        uchar* sptr = image;
        uint*  dptr = (uint*)img.bits();

        uint dim = width * height;

        for (uint i = 0; i < dim; ++i)
        {
            *dptr++ = qRgba(sptr[2], sptr[1], sptr[0], sptr[3]);
            sptr += 4;
        }
    }
    else
        qDebug() << "Failed to allocate memory for loading raw file";

    ProgressEvent * emitEvent = new ProgressEvent();
    emitEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Finishing...") ));
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), emitEvent);
    QCoreApplication::processEvents();

    emit imageLoaded(url, img);
}

void ImageLoadingThread::loadImage(const KUrl & url)
{
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
    buildImageEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Decoding image") ));
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), buildImageEvent);
    QCoreApplication::processEvents();

    QImage img = QImage::fromData(ba);

    ProgressEvent * emitEvent = new ProgressEvent();
    emitEvent->setData(ProgressEvent::ActionUpdate, QVariant( i18n("Finishing...") ));
    QCoreApplication::postEvent(PhotoLayoutsEditor::instance(), emitEvent);
    QCoreApplication::processEvents();

    emit imageLoaded(url, img);
}
