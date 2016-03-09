/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

KBImageLoader::KBImageLoader(QList<QPair<QString, int> >& fileList, int width, int height, bool loop)
    : QThread()
{
    m_initialized   = false;
    m_needImage     = true;
    m_haveImages    = false;
    m_quitRequested = false;
    m_fileIndex     = 0;
    m_fileList      = fileList;
    m_width         = width;
    m_height        = height;
    m_loop          = loop;
    m_textureAspect = 0.0;
}

void KBImageLoader::quit()
{
    QMutexLocker locker(&m_condLock);

    m_quitRequested = true;
    m_imageRequest.wakeOne();
}

void KBImageLoader::requestNewImage()
{
    QMutexLocker locker(&m_condLock);

    if ( !m_needImage)
    {
        m_needImage = true;
        m_imageRequest.wakeOne();
    }
}

void KBImageLoader::run()
{
    QMutexLocker locker(&m_condLock);

    // we enter the loop with m_needImage==true, so we will immediately
    // try to load an image

    while (true)
    {
        if (m_quitRequested)
            break;

        if (m_needImage)
        {
            if ( m_fileIndex == (int)m_fileList.count() )
            {
                if ( m_loop )
                {
                    m_fileIndex = 0;
                }
                else
                {
                    m_needImage = false;
                    emit(signalEndOfShow());
                    continue;
                }
            }

            m_needImage = false;
            m_condLock.unlock();
            bool ok;

            do
            {
                ok = loadImage();

                if ( !ok)
                    invalidateCurrentImageName();
            }
            while ( !ok && m_fileIndex < (int)m_fileList.count());

            if ( m_fileIndex == (int)m_fileList.count() )
            {

                emit(signalEndOfShow());
                m_condLock.lock();
                continue;
            }

            if ( !ok)
            {
                // generate a black dummy image
                m_texture = QImage(128, 128, QImage::Format_ARGB32);
                m_texture.fill(Qt::black);
            }

            m_condLock.lock();

            m_fileIndex++;

            if ( !m_initialized)
            {
                m_haveImages  = ok;
                m_initialized = true;
            }
        }
        else
        {
            // wait for new requests from the consumer
            m_imageRequest.wait(&m_condLock);
        }
    }
}

bool KBImageLoader::loadImage()
{
    QPair<QString, int> fileAngle = m_fileList[m_fileIndex];
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
    image        = image.scaled(m_width, m_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_imageLock.lock();

    m_textureAspect = aspect;

    // this is the critical moment, when we make the new texture and
    // aspect available to the consumer
    m_texture       = QGLWidget::convertToGLFormat(image);

    m_imageLock.unlock();

    return true;
}

void KBImageLoader::invalidateCurrentImageName()
{
    m_fileList.removeAll(m_fileList[m_fileIndex]);
    m_fileIndex++;
}

bool KBImageLoader::grabImage()
{
    m_imageLock.lock();
    return m_haveImages;
}

void KBImageLoader::ungrabImage()
{
    m_imageLock.unlock();
}

bool KBImageLoader::ready() const
{
    return m_initialized;
}

const QImage& KBImageLoader::image() const
{
    return m_texture;
}

float KBImageLoader::imageAspect() const
{
    return m_textureAspect;
}

}  // namespace KIPIAdvancedSlideshowPlugin
