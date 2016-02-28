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

#include "slideshowloader.h"

// Qt includes

#include <QImage>
#include <QMatrix>
#include <QPainter>
#include <QList>
#include <QFileInfo>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "commoncontainer.h"
#include "kpimageinfo.h"
#include "kipiplugins_debug.h"

using namespace KIPIPlugins;

namespace KIPIAdvancedSlideshowPlugin
{

LoadThread::LoadThread(LoadedImages* const loadedImages, QMutex* const imageLock, const QUrl& path,
                       int orientation, int width, int height)
    : QThread()
{
    m_path         = path;
    m_orientation  = orientation;
    m_swidth       = width;
    m_sheight      = height;
    m_imageLock    = imageLock;
    m_loadedImages = loadedImages;
    m_iface        = 0;

    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();
    }
}

LoadThread::~LoadThread()
{
}

void LoadThread::run()
{
    QImage newImage;

    // check if it's a RAW file.

    if (m_iface)
    {
        QPointer<RawProcessor> rawdec = m_iface->createRawProcessor();

        // check if its a RAW file.
        if (rawdec && rawdec->isRawFile(m_path))
        {
            rawdec->loadRawPreview(m_path, newImage);
        }
    }

    if (newImage.isNull())
    {
        // use the standard loader
        newImage = QImage(m_path.toLocalFile());
    }

    // Rotate according to orientation flag

    if ( m_iface &&  m_orientation != MetadataProcessor::UNSPECIFIED )
    {
        QPointer<MetadataProcessor> meta = m_iface->createMetadataProcessor();

        if (meta)
            meta->rotateExifQImage(newImage, m_orientation);
    }

    newImage = newImage.scaled(m_swidth, m_sheight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_imageLock->lock();
    m_loadedImages->insert(m_path, newImage);
    m_imageLock->unlock();
}

// -----------------------------------------------------------------------------------------

SlideShowLoader::SlideShowLoader(FileList& pathList, uint cacheSize, int width, int height,
                                 SharedContainer* const sharedData, int beginAtIndex)
{
    m_currIndex      = beginAtIndex;
    m_cacheSize      = cacheSize;
    m_pathList       = pathList;
    m_swidth         = width;
    m_sheight        = height;
    m_loadingThreads = new LoadingThreads();
    m_loadedImages   = new LoadedImages();
    m_imageLock      = new QMutex();
    m_threadLock     = new QMutex();
    m_sharedData     = sharedData;

    QUrl filePath;
    int  orientation;

    for (uint i = 0; i < uint(m_cacheSize / 2) && i < uint(m_pathList.count()); ++i)
    {
        filePath    = QUrl(m_pathList[i].first);
        KPImageInfo info(filePath);
        orientation = info.orientation();

        LoadThread* const newThread = new LoadThread(m_loadedImages, m_imageLock,
                                                     filePath, orientation, m_swidth, m_sheight);
        m_threadLock->lock();
        m_loadingThreads->insert(filePath, newThread);
        newThread->start();
        m_threadLock->unlock();
    }

    for (uint i = 0; i < (m_cacheSize % 2 == 0 ? (m_cacheSize % 2) : uint(m_cacheSize / 2) + 1); ++i)
    {
        int toLoad  = (m_currIndex - i) % m_pathList.count();
        filePath    = QUrl(m_pathList[toLoad].first);
        KPImageInfo info(filePath);
        orientation = info.orientation();

        LoadThread* const newThread = new LoadThread(m_loadedImages, m_imageLock,
                                                     filePath, orientation, m_swidth, m_sheight);
        m_threadLock->lock();
        m_loadingThreads->insert(filePath, newThread);
        newThread->start();
        m_threadLock->unlock();
    }
}

SlideShowLoader::~SlideShowLoader()
{
    m_threadLock->lock();
    LoadingThreads::Iterator it;

    for (it = m_loadingThreads->begin(); it != m_loadingThreads->end(); ++it)
    {
        // better check for a valid pointer here
        if (it.value())
            it.value()->wait();
        delete it.value();
        it = m_loadingThreads->erase(it);
    }

    m_threadLock->unlock();

    delete m_loadedImages;
    delete m_loadingThreads;
    delete m_imageLock;
    delete m_threadLock;
}

void SlideShowLoader::next()
{
    int victim = (m_currIndex - (m_cacheSize % 2 == 0 ? (m_cacheSize / 2) - 1
                                                      :  int(m_cacheSize / 2))) % m_pathList.count();

    int newBorn = (m_currIndex + int(m_cacheSize / 2) + 1) % m_pathList.count();

    m_currIndex = (m_currIndex + 1) % m_pathList.count();

    if (victim == newBorn)
        return;

    m_threadLock->lock();

    LoadThread* const oldThread = m_loadingThreads->value(QUrl::fromLocalFile(m_pathList[victim].first));

    if (oldThread)
        oldThread->wait();

    delete oldThread;

    m_loadingThreads->remove(QUrl::fromLocalFile(m_pathList[victim].first));
    m_imageLock->lock();
    m_loadedImages->remove(QUrl::fromLocalFile(m_pathList[victim].first));
    m_imageLock->unlock();
    m_threadLock->unlock();

    QUrl filePath   = QUrl::fromLocalFile((m_pathList[newBorn].first));
    KPImageInfo info(filePath);
    int orientation = info.orientation();

    LoadThread* const newThread = new LoadThread(m_loadedImages, m_imageLock, filePath, orientation, m_swidth, m_sheight);

    m_threadLock->lock();

    m_loadingThreads->insert(filePath, newThread);
    newThread->start();

    m_threadLock->unlock();
}

void SlideShowLoader::prev()
{
    int victim  = (m_currIndex + int(m_currIndex / 2)) % m_pathList.count();
    int newBorn = (m_currIndex - ((m_cacheSize & 2) == 0 ? (m_cacheSize / 2)
                                                         : int(m_cacheSize / 2) + 1)) % m_pathList.count();

    m_currIndex = m_currIndex > 0 ? m_currIndex - 1 : m_pathList.count() - 1;

    if (victim == newBorn)
        return;

    m_threadLock->lock();
    m_imageLock->lock();

    LoadThread* const oldThread = m_loadingThreads->value(QUrl::fromLocalFile(m_pathList[victim].first));

    if (oldThread)
        oldThread->wait();

    delete oldThread;

    m_loadingThreads->remove(QUrl::fromLocalFile(m_pathList[victim].first));
    m_loadedImages->remove(QUrl::fromLocalFile(m_pathList[victim].first));

    m_imageLock->unlock();
    m_threadLock->unlock();

    QUrl filePath   = QUrl::fromLocalFile(m_pathList[newBorn].first);
    KPImageInfo info(filePath);
    int orientation = info.orientation();

    LoadThread* const newThread = new LoadThread(m_loadedImages, m_imageLock, filePath, orientation, m_swidth, m_sheight);

    m_threadLock->lock();

    m_loadingThreads->insert(filePath, newThread);
    newThread->start();

    m_threadLock->unlock();
}

QImage SlideShowLoader::getCurrent()
{
    checkIsIn(m_currIndex);
    m_imageLock->lock();
    QImage returned = (*m_loadedImages)[QUrl::fromLocalFile(m_pathList[m_currIndex].first)];
    m_imageLock->unlock();

    return returned;
}

QString SlideShowLoader::currFileName() const
{
    return QUrl::fromLocalFile(m_pathList[m_currIndex].first).fileName();
}

QUrl SlideShowLoader::currPath() const
{
    return QUrl::fromLocalFile(m_pathList[m_currIndex].first);
}

void SlideShowLoader::checkIsIn(int index)
{
    m_threadLock->lock();

    if (m_loadingThreads->contains(QUrl::fromLocalFile(m_pathList[index].first)))
    {
        if ((*m_loadingThreads)[QUrl::fromLocalFile(m_pathList[index].first)]->isRunning())
            (*m_loadingThreads)[QUrl::fromLocalFile(m_pathList[index].first)]->wait();

        m_threadLock->unlock();
    }
    else
    {
        QUrl filePath   = QUrl(m_pathList[index].first);
        KPImageInfo info(filePath);
        int orientation = info.orientation();

        LoadThread* const newThread = new LoadThread(m_loadedImages, m_imageLock, filePath, orientation, m_swidth, m_sheight);

        m_loadingThreads->insert(QUrl::fromLocalFile(m_pathList[index].first), newThread);
        newThread->start();
        (*m_loadingThreads)[QUrl::fromLocalFile(m_pathList[index].first)]->wait();
        m_threadLock->unlock();
    }
}

} // namespace KIPIAdvancedSlideshowPlugin
