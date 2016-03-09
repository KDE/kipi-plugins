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
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <KIPI/Interface>

// Local includes

#include "commoncontainer.h"
#include "kpimageinfo.h"
#include "kipiplugins_debug.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIAdvancedSlideshowPlugin
{

typedef QMap<QUrl, QImage> LoadedImages;

class LoadThread : public QThread
{

public:

    LoadThread(LoadedImages* const loadedImages, QMutex* const imageLock, const QUrl& path,
               int orientation, int width, int height)
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

    ~LoadThread()
    {
    }

protected:

    void run()
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

private:

    QMutex*       m_imageLock;
    LoadedImages* m_loadedImages;
    QUrl          m_path;
    QString       m_filename;
    int           m_orientation;
    int           m_swidth;
    int           m_sheight;
    Interface*    m_iface;
};

typedef QMap<QUrl, LoadThread*> LoadingThreads;
    
// -----------------------------------------------------------------------------------------

class SlideShowLoader::Private
{

public:

    Private()
    {
        loadingThreads = 0;
        loadedImages   = 0;
        imageLock      = 0;
        threadLock     = 0;
        sharedData     = 0;
        cacheSize      = 0;
        currIndex      = 0;
        swidth         = 0;
        sheight        = 0;
    }

    LoadingThreads*  loadingThreads;
    LoadedImages*    loadedImages;
    FileList         pathList;

    QMutex*          imageLock;
    QMutex*          threadLock;

    SharedContainer* sharedData;

    uint             cacheSize;
    int              currIndex;
    int              swidth;
    int              sheight;
};

SlideShowLoader::SlideShowLoader(FileList& pathList, uint cacheSize, int width, int height,
                                 SharedContainer* const sharedData, int beginAtIndex)
    : d(new Private)
{
    d->currIndex      = beginAtIndex;
    d->cacheSize      = cacheSize;
    d->pathList       = pathList;
    d->swidth         = width;
    d->sheight        = height;
    d->loadingThreads = new LoadingThreads();
    d->loadedImages   = new LoadedImages();
    d->imageLock      = new QMutex();
    d->threadLock     = new QMutex();
    d->sharedData     = sharedData;

    QUrl filePath;
    int  orientation;

    for (uint i = 0; i < uint(d->cacheSize / 2) && i < uint(d->pathList.count()); ++i)
    {
        filePath    = QUrl::fromLocalFile(d->pathList[i].first);
        KPImageInfo info(filePath);
        orientation = info.orientation();

        LoadThread* const newThread = new LoadThread(d->loadedImages, d->imageLock,
                                                     filePath, orientation, d->swidth, d->sheight);
        d->threadLock->lock();
        d->loadingThreads->insert(filePath, newThread);
        newThread->start();
        d->threadLock->unlock();
    }

    for (uint i = 0; i < (d->cacheSize % 2 == 0 ? (d->cacheSize % 2) : uint(d->cacheSize / 2) + 1); ++i)
    {
        int toLoad  = (d->currIndex - i) % d->pathList.count();
        filePath    = QUrl::fromLocalFile(d->pathList[toLoad].first);
        KPImageInfo info(filePath);
        orientation = info.orientation();

        LoadThread* const newThread = new LoadThread(d->loadedImages, d->imageLock,
                                                     filePath, orientation, d->swidth, d->sheight);
        d->threadLock->lock();
        d->loadingThreads->insert(filePath, newThread);
        newThread->start();
        d->threadLock->unlock();
    }
}

SlideShowLoader::~SlideShowLoader()
{
    d->threadLock->lock();
    LoadingThreads::Iterator it;

    for (it = d->loadingThreads->begin(); it != d->loadingThreads->end(); ++it)
    {
        // better check for a valid pointer here
        if (it.value())
            it.value()->wait();

        delete it.value();
    }

    d->loadingThreads->clear();

    d->threadLock->unlock();

    delete d->loadedImages;
    delete d->loadingThreads;
    delete d->imageLock;
    delete d->threadLock;
    delete d;
}

void SlideShowLoader::next()
{
    int victim = (d->currIndex - (d->cacheSize % 2 == 0 ? (d->cacheSize / 2) - 1
                                                      :  int(d->cacheSize / 2))) % d->pathList.count();

    int newBorn = (d->currIndex + int(d->cacheSize / 2) + 1) % d->pathList.count();

    d->currIndex = (d->currIndex + 1) % d->pathList.count();

    if (victim == newBorn)
        return;

    d->threadLock->lock();

    LoadThread* const oldThread = d->loadingThreads->value(QUrl::fromLocalFile(d->pathList[victim].first));

    if (oldThread)
        oldThread->wait();

    delete oldThread;

    d->loadingThreads->remove(QUrl::fromLocalFile(d->pathList[victim].first));
    d->imageLock->lock();
    d->loadedImages->remove(QUrl::fromLocalFile(d->pathList[victim].first));
    d->imageLock->unlock();
    d->threadLock->unlock();

    QUrl filePath   = QUrl::fromLocalFile((d->pathList[newBorn].first));
    KPImageInfo info(filePath);
    int orientation = info.orientation();

    LoadThread* const newThread = new LoadThread(d->loadedImages, d->imageLock, filePath, orientation, d->swidth, d->sheight);

    d->threadLock->lock();

    d->loadingThreads->insert(filePath, newThread);
    newThread->start();

    d->threadLock->unlock();
}

void SlideShowLoader::prev()
{
    int victim  = (d->currIndex + int(d->currIndex / 2)) % d->pathList.count();
    int newBorn = (d->currIndex - ((d->cacheSize & 2) == 0 ? (d->cacheSize / 2)
                                                         : int(d->cacheSize / 2) + 1)) % d->pathList.count();

    d->currIndex = d->currIndex > 0 ? d->currIndex - 1 : d->pathList.count() - 1;

    if (victim == newBorn)
        return;

    d->threadLock->lock();
    d->imageLock->lock();

    LoadThread* const oldThread = d->loadingThreads->value(QUrl::fromLocalFile(d->pathList[victim].first));

    if (oldThread)
        oldThread->wait();

    delete oldThread;

    d->loadingThreads->remove(QUrl::fromLocalFile(d->pathList[victim].first));
    d->loadedImages->remove(QUrl::fromLocalFile(d->pathList[victim].first));

    d->imageLock->unlock();
    d->threadLock->unlock();

    QUrl filePath   = QUrl::fromLocalFile(d->pathList[newBorn].first);
    KPImageInfo info(filePath);
    int orientation = info.orientation();

    LoadThread* const newThread = new LoadThread(d->loadedImages, d->imageLock, filePath, orientation, d->swidth, d->sheight);

    d->threadLock->lock();

    d->loadingThreads->insert(filePath, newThread);
    newThread->start();

    d->threadLock->unlock();
}

QImage SlideShowLoader::getCurrent() const
{
    checkIsIn(d->currIndex);
    d->imageLock->lock();
    QImage returned = (*d->loadedImages)[QUrl::fromLocalFile(d->pathList[d->currIndex].first)];
    d->imageLock->unlock();

    return returned;
}

QString SlideShowLoader::currFileName() const
{
    return QUrl::fromLocalFile(d->pathList[d->currIndex].first).fileName();
}

QUrl SlideShowLoader::currPath() const
{
    return QUrl::fromLocalFile(d->pathList[d->currIndex].first);
}

void SlideShowLoader::checkIsIn(int index) const
{
    d->threadLock->lock();

    if (d->loadingThreads->contains(QUrl::fromLocalFile(d->pathList[index].first)))
    {
        if ((*d->loadingThreads)[QUrl::fromLocalFile(d->pathList[index].first)]->isRunning())
            (*d->loadingThreads)[QUrl::fromLocalFile(d->pathList[index].first)]->wait();

        d->threadLock->unlock();
    }
    else
    {
        QUrl filePath   = QUrl::fromLocalFile(d->pathList[index].first);
        KPImageInfo info(filePath);
        int orientation = info.orientation();

        LoadThread* const newThread = new LoadThread(d->loadedImages, d->imageLock, filePath, orientation, d->swidth, d->sheight);

        d->loadingThreads->insert(QUrl::fromLocalFile(d->pathList[index].first), newThread);
        newThread->start();
        (*d->loadingThreads)[QUrl::fromLocalFile(d->pathList[index].first)]->wait();
        d->threadLock->unlock();
    }
}

} // namespace KIPIAdvancedSlideshowPlugin
