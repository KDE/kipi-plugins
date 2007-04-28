/* ============================================================
 * File  : collection.h
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

#ifndef KIPI2_COLLECTION_H
#define KIPI2_COLLECTION_H

#include <qvaluelist.h>

namespace KIPI2
{
  enum Features
  {
    None = 0,
    Comments = 1,
    SubCollections = 2,
    ItemComments = 4,
    NewSubCollections = 8,
    NewItems = 16
  };
  
  // Forward declarations
  class Collection;
  class Item;
  
  // Some list definitions
  typedef QValueList<Collection*> CollectionList;
  typedef QValueList<Item*> ItemList;
  
  // And some templated functions for cleaning up in Collection derived destructors
  template <class T> void DestroyCollectionList(CollectionList& collections);
  template <class T> void DestroyItemList(ItemList& items);
  
  // Main (abstract) Collection definition
  class Collection
  {
  public:
    // ctor/dtor
    Collection(Collection* pParent = NULL, unsigned int mFeatures = 0);
    
    // State whether a given Feature is supported.
    bool Supports(enum Features feature);
    
    const Collection* getParentCollection();
    
    // Get the list of Sub-Collections (assuming this is supported)
    virtual const CollectionList* getSubCollections();
    // Create a new sub collection
    virtual const Collection* createSubCollection() = 0;
    
    // Get the list of items.
    virtual const ItemList* getItems();
    // Add a new item
    virtual const Item* addItem() = 0;
  
  private:
    // Store for parent
    Collection* mpParent;
    
    // Store the features that this collection supports
    unsigned int mFeatures;
    
    // Store for the Sub-Collections
    CollectionList* mpSubCollections;
    
    // Store for the Items
    ItemList* mpItems;
  };
}
#endif /* KIPI2_COLLECTION_H */
