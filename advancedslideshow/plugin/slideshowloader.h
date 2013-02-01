/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-11
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef SLIDESHOWLOADER_H
#define SLIDESHOWLOADER_H

// Qt includes

#include <QList>
#include <QMap>
#include <QMutex>
#include <QImage>
#include <QThread>
#include <QPair>

// KDE includes

#include <kurl.h>

// Local includes

#include "kpmetadata.h"

typedef QPair<QString, int>  FileAnglePair;
typedef QList<FileAnglePair> FileList;

using namespace KIPIPlugins;

namespace KIPIAdvancedSlideshowPlugin
{

typedef QMap<KUrl, QImage> LoadedImages;

class SharedContainer;

class LoadThread : public QThread
{

public:

    LoadThread(LoadedImages* const loadedImages, QMutex* const imageLock, const KUrl& path,
               KPMetadata::ImageOrientation orientation, int width, int height);
    ~LoadThread();

protected:

    void run();

private:

    QMutex*                      m_imageLock;
    LoadedImages*                m_loadedImages;
    KUrl                         m_path;
    QString                      m_filename;
    KPMetadata::ImageOrientation m_orientation;
    int                          m_swidth;
    int                          m_sheight;
};

// ----------------------------------------------------------------------------

typedef QMap<KUrl, LoadThread*> LoadingThreads;

class SlideShowLoader
{

public:

    SlideShowLoader(FileList& pathList, uint cacheSize, int width, int height,
                    SharedContainer* const sharedData, int beginAtIndex = 0);
    ~SlideShowLoader();

    void    next();
    void    prev();

    QImage  getCurrent();
    QString currFileName() const;
    KUrl    currPath() const;

private:

    void checkIsIn(int index);

private:

    LoadingThreads*  m_loadingThreads;
    LoadedImages*    m_loadedImages;
    FileList         m_pathList;

    QMutex*          m_imageLock;
    QMutex*          m_threadLock;

    SharedContainer* m_sharedData;

    uint             m_cacheSize;
    int              m_currIndex;
    int              m_swidth;
    int              m_sheight;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif // SLIDESHOWLOADER_H
