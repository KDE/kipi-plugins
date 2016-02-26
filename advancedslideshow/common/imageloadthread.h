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

#ifndef IMAGELOADTHREAD_H
#define IMAGELOADTHREAD_H

// Qt includes

#include <QList>
#include <QImage>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QPair>
#include <QString>

namespace KIPIAdvancedSlideshowPlugin
{

class ImageLoadThread : public QThread
{
    Q_OBJECT

public:

    ImageLoadThread(QList<QPair<QString, int> >& fileList, int width, int height, bool loop);

    void  quit();
    void  requestNewImage();

    bool  grabImage()
    {
        m_imageLock.lock();
        return m_haveImages;
    };

    void  ungrabImage()
    {
        m_imageLock.unlock();
    };

    bool  ready() const
    {
        return m_initialized;
    };

    const QImage& image() const
    {
        return m_texture;
    };

    float imageAspect() const
    {
        return m_textureAspect;
    };

Q_SIGNALS:

    void signalEndOfShow();

protected:

    void run();
    bool loadImage();
    void invalidateCurrentImageName();

private:

    int                         m_fileIndex;
    QList<QPair<QString, int> > m_fileList;

    int                         m_width, m_height;

    QWaitCondition              m_imageRequest;
    QMutex                      m_condLock, m_imageLock;
    bool                        m_initialized, m_needImage, m_haveImages, m_quitRequested, m_loop;

    float                       m_textureAspect;
    QImage                      m_texture;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif // IMAGELOADTHREAD_H
