/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold
 * <carsten dot weinhold at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kbimageloader.h"

// Qt includes

#include <QMatrix>
#include <QFileInfo>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "kipiplugins_debug.h"
#include "slideshowkb.h"

using namespace KIPI;

namespace KIPIAdvancedSlideshowPlugin
{

class KBImageLoader::Private
{

public:

    Private()
    {
        fileIndex     = 0;
        width         = 0;
        height        = 0;
        initialized   = false;
        needImage     = true;
        haveImages    = false;
        quitRequested = false;
        loop          = false;
        textureAspect = 0.0;
    }

    int                         fileIndex;
    QList<QPair<QString, int> > fileList;

    int                         width;
    int                         height;

    QWaitCondition              imageRequest;
    QMutex                      condLock;
    QMutex                      imageLock;

    bool                        initialized;
    bool                        needImage;
    bool                        haveImages;
    bool                        quitRequested;
    bool                        loop;

    float                       textureAspect;
    QImage                      texture;
};

KBImageLoader::KBImageLoader(QList<QPair<QString, int> >& fileList, int width, int height, bool loop)
    : QThread(),
      d(new Private)
{
    d->fileList = fileList;
    d->width    = width;
    d->height   = height;
    d->loop     = loop;
}

KBImageLoader::~KBImageLoader()
{
    delete d;
}

void KBImageLoader::quit()
{
    QMutexLocker locker(&d->condLock);

    d->quitRequested = true;
    d->imageRequest.wakeOne();
}

void KBImageLoader::requestNewImage()
{
    QMutexLocker locker(&d->condLock);

    if ( !d->needImage)
    {
        d->needImage = true;
        d->imageRequest.wakeOne();
    }
}

void KBImageLoader::run()
{
    QMutexLocker locker(&d->condLock);

    // we enter the loop with d->needImage==true, so we will immediately
    // try to load an image

    while (true)
    {
        if (d->quitRequested)
            break;

        if (d->needImage)
        {
            if ( d->fileIndex == (int)d->fileList.count() )
            {
                if ( d->loop )
                {
                    d->fileIndex = 0;
                }
                else
                {
                    d->needImage = false;
                    emit(signalEndOfShow());
                    continue;
                }
            }

            d->needImage = false;
            d->condLock.unlock();
            bool ok;

            do
            {
                ok = loadImage();

                if ( !ok)
                    invalidateCurrentImageName();
            }
            while ( !ok && d->fileIndex < (int)d->fileList.count());

            if ( d->fileIndex == (int)d->fileList.count() )
            {

                emit(signalEndOfShow());
                d->condLock.lock();
                continue;
            }

            if ( !ok)
            {
                // generate a black dummy image
                d->texture = QImage(128, 128, QImage::Format_ARGB32);
                d->texture.fill(Qt::black);
            }

            d->condLock.lock();

            d->fileIndex++;

            if ( !d->initialized)
            {
                d->haveImages  = ok;
                d->initialized = true;
            }
        }
        else
        {
            // wait for new requests from the consumer
            d->imageRequest.wait(&d->condLock);
        }
    }
}

bool KBImageLoader::loadImage()
{
    QPair<QString, int> fileAngle = d->fileList[d->fileIndex];
    QString             path(fileAngle.first);
    int                 angle(fileAngle.second);
    QImage              image;

    // check if it's a RAW file.

    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        Interface* const iface = pl->interface();

        if (iface)
        {
            QPointer<RawProcessor> rawdec = iface->createRawProcessor();

            // check if its a RAW file.
            if (rawdec && rawdec->isRawFile(QUrl::fromLocalFile(path)))
            {
                rawdec->loadRawPreview(QUrl::fromLocalFile(path), image);
            }
        }
    }

    if (image.isNull())
    {
        // use the standard loader
        image = QImage(path);
    }

    if (image.isNull())
    {
        return false;
    }

    if (angle != 0)
    {
        QMatrix wm;
        wm.rotate(angle);
        image = image.transformed(wm);
    }

    float aspect = (float)image.width() / (float)image.height();
    image        = image.scaled(d->width, d->height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    d->imageLock.lock();

    d->textureAspect = aspect;

    // this is the critical moment, when we make the new texture and
    // aspect available to the consumer
    d->texture       = QGLWidget::convertToGLFormat(image);

    d->imageLock.unlock();

    return true;
}

void KBImageLoader::invalidateCurrentImageName()
{
    d->fileList.removeAll(d->fileList[d->fileIndex]);
    d->fileIndex++;
}

bool KBImageLoader::grabImage()
{
    d->imageLock.lock();
    return d->haveImages;
}

void KBImageLoader::ungrabImage()
{
    d->imageLock.unlock();
}

bool KBImageLoader::ready() const
{
    return d->initialized;
}

const QImage& KBImageLoader::image() const
{
    return d->texture;
}

float KBImageLoader::imageAspect() const
{
    return d->textureAspect;
}

}  // namespace KIPIAdvancedSlideshowPlugin
