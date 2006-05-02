/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at free.fr>
 * Date   : 2004-11-09
 * Description : Multithread queue description class 
 *
 * Copyright 2004-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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

#ifndef MTQUEUE_H
#define MTQUEUE_H

// Qt includes.

#include <qptrqueue.h>
#include <qmutex.h>

namespace KIPIJPEGLossLessPlugin
{

template<class Type> class MTQueue
{

public:

  MTQueue()
  {
      queue_.setAutoDelete(true);
  }

  ~MTQueue()
  {
    flush();
  }

  bool isEmpty()
  {
    mutex_.lock();
    bool empty = queue_.isEmpty();
    mutex_.unlock();
    return empty;
  }

  void flush()
  {
    mutex_.lock();
    queue_.clear();
    mutex_.unlock();
  }

  void enqueue(Type * t)
  {
    mutex_.lock();
    queue_.enqueue(t);
    mutex_.unlock();
  }

  Type * dequeue()
  {
    mutex_.lock();
    Type * i = queue_.dequeue();
    mutex_.unlock();
    return i;
  }

private:

  QPtrQueue<Type> queue_;
  QMutex          mutex_;
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif // KIPIJPEGLossLessPlugin
