/* ============================================================
 * File  : slideshowloader.cpp
 * Author: Valerio Fuoglio <valerio.fuoglio@gmail.com>
 * Date  : 2007-11-11
 * Description : 
 * 
 * Copyright 2007 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// KDE includes
#include <kdebug.h>

// QT includes
#include <qimage.h>
#include <qwmatrix.h>
#include <qpainter.h>
#include <qvaluelist.h>

// Local includes
#include "slideshowloader.h"

namespace KIPISlideShowPlugin
{ 
  
  // ---------------------------- LoadThread Class ----------------------- //
  
  LoadThread::LoadThread(LoadedImages* loadedImages, QMutex* imageLock, const KURL path, 
                         const int angle, int width, int height) {

    m_path  =  path;
    m_angle =  angle;
    m_swidth = width; 
    m_sheight = height; 
    m_imageLock = imageLock;
    m_loadedImages = loadedImages;
  }
  
  LoadThread::~LoadThread() {
  }
  
  void LoadThread::run() {
  
    QImage newImage(m_path.path());
    // Rotate according to angle
    if ( m_angle != 0 ) 
    {
      QWMatrix matrix;
      matrix.rotate((double)m_angle);
      newImage.xForm( matrix );
    }
    
  
    newImage = QImage(newImage.smoothScale(m_swidth, m_sheight, QImage::ScaleMin));
    
    m_imageLock->lock();
    m_loadedImages->insert(m_path, newImage);
    m_imageLock->unlock();
  }

  // ----------------------- SlideShowLoader Class ----------------------- //
  SlideShowLoader::SlideShowLoader(FileList &pathList, uint cacheSize, int width, int height, int beginAtIndex) {

    m_currIndex = beginAtIndex;
    m_cacheSize = cacheSize;
    m_pathList = pathList;
    m_swidth = width;
    m_sheight = height;
    m_loadingThreads = new LoadingThreads();
    m_loadedImages = new LoadedImages();
    m_imageLock = new QMutex();
    m_threadLock = new QMutex();
    
    for ( uint i = 0; i < uint(m_cacheSize/2) && i < uint(m_pathList.count()); i++ ) {
      LoadThread* newThread = new LoadThread(m_loadedImages, m_imageLock, KURL(m_pathList[i].first), 
                                             m_pathList[i].second, m_swidth, m_sheight);
      m_threadLock->lock();
      m_loadingThreads->insert(m_pathList[i].first, newThread);
      newThread->start(); 
      m_threadLock->unlock();
    }  
    for ( uint i = 0; i < ( m_cacheSize%2 == 0? (m_cacheSize%2) : uint(m_cacheSize/2)+1); i++ ) {
      int toLoad = (m_currIndex - i)%m_pathList.count();
      LoadThread* newThread = new LoadThread(m_loadedImages, m_imageLock, KURL(m_pathList[toLoad].first), 
                                             m_pathList[toLoad].second, m_swidth, m_sheight);
      m_threadLock->lock();
      m_loadingThreads->insert(m_pathList[toLoad].first, newThread);
      newThread->start(); 
      m_threadLock->unlock();
    }
    
  }

  SlideShowLoader::~SlideShowLoader() {
    
    m_threadLock->lock();
   LoadingThreads::Iterator it;
    for ( it = m_loadingThreads->begin(); it != m_loadingThreads->end(); ++it ) {
      it.data()->wait();
      delete it.data();
      m_loadingThreads->remove(it);
    }
    m_threadLock->unlock();

    
    delete(m_loadingThreads);
    delete(m_loadedImages);
 
    delete(m_imageLock);
    delete(m_threadLock);
  }
  
  void SlideShowLoader::next() {
    int victim = (m_currIndex - ( m_cacheSize%2 == 0 ? (m_cacheSize/2)-1 : int(m_cacheSize/2))) % m_pathList.count();
    int newBorn = (m_currIndex + int(m_cacheSize/2) + 1) % m_pathList.count();
    
    if ( victim == newBorn ) return;
    
    m_threadLock->lock();
    m_imageLock->lock();
    m_loadingThreads->remove(m_pathList[victim].first);
    m_loadedImages->remove(m_pathList[victim].first);
    m_imageLock->unlock();
    m_threadLock->unlock();
    
    LoadThread* newThread = new LoadThread(m_loadedImages, m_imageLock, KURL(m_pathList[newBorn].first), 
                                           m_pathList[newBorn].second, m_swidth, m_sheight);
    
    m_threadLock->lock();
    m_loadingThreads->insert(m_pathList[newBorn].first, newThread);
    newThread->start();
    m_threadLock->unlock();
    
    m_currIndex = (m_currIndex + 1) % m_pathList.count();
  }
  
  void SlideShowLoader::prev() {
    int victim = (m_currIndex + int(m_currIndex/2)) % m_pathList.count();
    int newBorn = (m_currIndex - ( m_cacheSize&2 == 0 ? (m_cacheSize/2) : int(m_cacheSize/2)+1)) % m_pathList.count();
    
    if ( victim == newBorn ) return;
    
    m_threadLock->lock();
    m_imageLock->lock();
    m_loadingThreads->remove(m_pathList[victim].first);
    m_loadedImages->remove(m_pathList[victim].first);
    m_imageLock->unlock();
    m_threadLock->unlock();
    
    LoadThread* newThread = new LoadThread(m_loadedImages, m_imageLock, KURL(m_pathList[newBorn].first), 
                                           m_pathList[newBorn].second, m_swidth, m_sheight);

    
    m_threadLock->lock();
    m_loadingThreads->insert(m_pathList[newBorn].first, newThread);
    newThread->start();
    m_threadLock->unlock();
    
    m_currIndex = (m_currIndex - 1) % m_pathList.count();
    
  }
  
  QImage SlideShowLoader::getCurrent() {
    checkIsIn(m_currIndex);
    
    m_imageLock->lock();
    QImage returned = (*m_loadedImages)[m_pathList[m_currIndex].first];
    m_imageLock->unlock(); 
    
    
    return returned;
  }
  
  QString SlideShowLoader::currFileName() 
  {
    return KURL(m_pathList[m_currIndex].first).fileName();
  }
  
  KURL SlideShowLoader::currPath() 
  {
    return m_pathList[m_currIndex].first;
  }
  
  void SlideShowLoader::checkIsIn(int index) 
  {
    m_threadLock->lock();
    if (m_loadingThreads->contains(m_pathList[index].first)) 
    {
      if ( (*m_loadingThreads)[m_pathList[index].first]->running() )
        (*m_loadingThreads)[m_pathList[index].first]->wait();
      m_threadLock->unlock();
    }
    else
    {
      LoadThread* newThread = new LoadThread(m_loadedImages, m_imageLock, KURL(m_pathList[index].first), 
                                             m_pathList[index].second, m_swidth, m_sheight);
      
      m_loadingThreads->insert(m_pathList[index].first,newThread);
      newThread->start();
      (*m_loadingThreads)[m_pathList[index].first]->wait();
      m_threadLock->unlock();
    }
    
  }

}  // NameSpace KIPISlideShowPlugin
