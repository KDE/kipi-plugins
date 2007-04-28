/* ============================================================
 * File  : interface.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-26
 *
 * Copyright 2007 by Colin Guthrie <kde@colin.guthr.ie>
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

#ifndef KIPI2_INTERFACE_H
#define KIPI2_INTERFACE_H

#include <qobject.h>
#include "collection.h"

namespace KIPI2
{
class Interface : public QObject
{
  Q_OBJECT

public:
  virtual const CollectionList* getCollections() = 0;
};
}
#endif /* KIPI2_INTERFACE_H */
