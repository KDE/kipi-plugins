/* ============================================================
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2004 by Tudor Calin <tudor@1xtech.com>

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
#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

#include <qptrqueue.h>
#include <qmutex.h>

namespace KIPIKameraKlientPlugin
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
  QMutex mutex_;

};

}  // NameSpace KIPIKameraKlientPlugin

#endif
