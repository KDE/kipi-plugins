/* ============================================================
 * File  : collection.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-26
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

#include "collection.h"

namespace KIPI2
{

template <class T> void DestroyCollectionList(CollectionList& collections)
{
  for (CollectionList::iterator it = collections.begin();
        it != collections.end();
        ++it)
  {
    delete dynamic_cast<T*>(*it);
  }
  collections.empty();
}

template <class T> void DestroyItemList(ItemList& items)
{
  for (ItemList::iterator it = items.begin();
        it != items.end();
        ++it)
  {
    delete dynamic_cast<T*>(*it);
  }
  items.empty();
}  


Collection::Collection(Collection* pParent, unsigned int features)
 : mpParent(pParent),
   mFeatures(features),
   mpSubCollections(NULL),
   mpItems(NULL)
{
}

bool Collection::Supports(enum Features feature)
{
  return (mFeatures & feature);
}

const Collection* Collection::getParentCollection()
{
  return mpParent;
}

// Default, basic implementation of getSubCollections
const CollectionList* Collection::getSubCollections()
{
  return mpSubCollections;
}


// Default, basic implementation of getItems
const ItemList* Collection::getItems()
{
  return mpItems;
}

}
