/* ============================================================
 * File  : sinkfactory.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-22
 *
 * Copyright 2007 Colin Guthrie
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ============================================================ */

#ifndef SINKFACTORY_H
#define SINKFACTORY_H

#include <qmap.h>
class QListView;

namespace KIPISyncPlugin
{
class Sink;

// Create a templated creator function to allow our "sinks" to
// register themselves with the factory.
template <class T>
Sink* SinkCreator()
{
  return (Sink*) new T;
}

// Define a type that is a pointer to the templated function above.
typedef Sink* SinkProxy();

// And define a type that is a Map of names to creator functions.
typedef QMap<QString, SinkProxy*> SinkMap;

// The factory to register and create the sinks
class SinkFactory
{
public:
  static bool Register(QString name, SinkProxy* pSinkProxy);
  static Sink* Create(QString name);

private:
  static SinkMap mSinkProxies;
};

}

#endif /* SINKFACTORY_H */
