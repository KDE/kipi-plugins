/* ============================================================
 *
 * Date        : 2010-06-21
 * Description : Test for RG tag model
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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
 *
 * ============================================================ */

#include "test_rgtagmodel.moc"

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// local includes

#include "simpletreemodel/simpletreemodel.h"
#include "../borrowed/modeltest.h"
#include "../rgtagmodel.h"

QTEST_KDEMAIN_CORE(TestRGTagModel)

/**
 * @brief Dummy test that does nothing
 */
void TestRGTagModel::testNoOp()
{
}

/**
 * @brief Create an RGTagModel, but leave it empty
 */
void TestRGTagModel::testModelEmpty()
{
    SimpleTreeModel* const treeModel = new SimpleTreeModel(1, this);
    new ModelTest(treeModel, this);

    KIPIGPSSyncPlugin::RGTagModel* const tagModel = new KIPIGPSSyncPlugin::RGTagModel(treeModel, this);
    new ModelTest(tagModel, this);
}

void TestRGTagModel::testModel1()
{
    SimpleTreeModel* const treeModel = new SimpleTreeModel(1, this);
    new ModelTest(treeModel, this);

    // add some items before the tagModel is created:
    SimpleTreeModel::Item* const treeItem1 = treeModel->addItem();
    QPersistentModelIndex treeItem1Index = treeModel->itemToIndex(treeItem1);
    SimpleTreeModel::Item* const treeItem11 = treeModel->addItem(treeItem1);
    QPersistentModelIndex treeItem11Index = treeModel->itemToIndex(treeItem11);

    KIPIGPSSyncPlugin::RGTagModel* const tagModel = new KIPIGPSSyncPlugin::RGTagModel(treeModel, this);
    // TODO: make sure the ModelTest does not find any errors, currently it does find errors ;-)
    new ModelTest(tagModel, this);
    
    // simple tests
    Q_ASSERT(tagModel->rowCount()==treeModel->rowCount());

    const QPersistentModelIndex tagItem1Index = tagModel->fromSourceIndex(treeItem1Index);
    Q_ASSERT(tagItem1Index.isValid());
    kDebug()<<tagItem1Index;
    
    Q_ASSERT(tagModel->rowCount(tagItem1Index)==treeModel->rowCount(treeItem1Index));
    
    
    // make sure the tagModel handles items inserted after it was created
    // - both top level
    SimpleTreeModel::Item* const treeItem2 = treeModel->addItem();
    QPersistentModelIndex treeItem2Index = treeModel->itemToIndex(treeItem2);
    Q_ASSERT(tagModel->rowCount()==treeModel->rowCount());
    const QPersistentModelIndex tagItem2Index = tagModel->fromSourceIndex(treeItem2Index);

    // - and sub items:
    SimpleTreeModel::Item* const treeItem21 = treeModel->addItem(treeItem2);
    Q_ASSERT(tagItem2Index.isValid());

    Q_ASSERT(tagModel->rowCount(tagItem2Index)==treeModel->rowCount(treeItem2Index));

 
    const QPersistentModelIndex tagItem11Index = tagModel->fromSourceIndex(treeItem11Index);
    Q_ASSERT(tagItem11Index.isValid());
    
    QPersistentModelIndex treeItem21Index = treeModel->itemToIndex(treeItem21);
    const QPersistentModelIndex tagItem21Index = tagModel->fromSourceIndex(treeItem21Index);
    Q_ASSERT(tagItem21Index.isValid());
   

    // now make sure we can descend:
    const QModelIndex ti1 = tagModel->index(0, 0);
    Q_ASSERT(ti1.isValid());
    Q_ASSERT(ti1 == tagItem1Index);

    // descends level 1 row 0
    const QModelIndex ti11 = tagModel->index(0, 0, ti1);
    Q_ASSERT(ti11.isValid());
    Q_ASSERT(ti11 == tagItem11Index); 
    kDebug()<<"----------------------_";
    // descends level 0 row 1
    const QModelIndex ti2 = tagModel->index(1, 0);
    Q_ASSERT(ti2.isValid());
    Q_ASSERT(ti2 == tagItem2Index); 
    
    // descends level 1 row 0
    QModelIndex ti21 = tagModel->index(0, 0, ti2);
    Q_ASSERT(ti21.isValid());
    Q_ASSERT(ti21 == tagItem21Index);

    //checks invalid index
    const QModelIndex ti111 = tagModel->index(0,0, ti11);
    Q_ASSERT(!ti111.isValid());

    //checks parent of tagItem1
    const QModelIndex parent_ti1 = tagModel->parent(ti1);
    Q_ASSERT(!parent_ti1.isValid());

    //checks parent of tagItem11
    const QModelIndex parent_ti11 = tagModel->parent(ti11);
    Q_ASSERT(parent_ti11 == tagItem1Index);
   
    //checks parent of tagItem2 
    const QModelIndex parent_ti2 = tagModel->parent(ti2);
    Q_ASSERT(!parent_ti2.isValid());


    const QModelIndex parent_ti21 = tagModel->parent(ti21);
    Q_ASSERT(parent_ti21.isValid());
}

void TestRGTagModel::testModelSpacerTags()
{
    SimpleTreeModel* const treeModel = new SimpleTreeModel(1, this);
    new ModelTest(treeModel, this);

    // add some items before the tagModel is created:
    SimpleTreeModel::Item* const treeItem1 = treeModel->addItem();
    QPersistentModelIndex treeItem1Index = treeModel->itemToIndex(treeItem1);
    SimpleTreeModel::Item* const treeItem11 = treeModel->addItem(treeItem1);
    QPersistentModelIndex treeItem11Index = treeModel->itemToIndex(treeItem11);

    KIPIGPSSyncPlugin::RGTagModel* const tagModel = new KIPIGPSSyncPlugin::RGTagModel(treeModel, this);
    // TODO: make sure the ModelTest does not find any errors, currently it does find errors ;-)
    new ModelTest(tagModel, this);

    SimpleTreeModel::Item* const treeItem2 = treeModel->addItem();
    QPersistentModelIndex treeItem2Index = treeModel->itemToIndex(treeItem2);
    Q_ASSERT(tagModel->rowCount()==treeModel->rowCount());
    const QPersistentModelIndex tagItem2Index = tagModel->fromSourceIndex(treeItem2Index);

    // - and sub items:
    SimpleTreeModel::Item* const treeItem21 = treeModel->addItem(treeItem2);
    Q_ASSERT(tagItem2Index.isValid());

    Q_ASSERT(tagModel->rowCount(tagItem2Index)==treeModel->rowCount(treeItem2Index));


    const QPersistentModelIndex tagItem11Index = tagModel->fromSourceIndex(treeItem11Index);
    Q_ASSERT(tagItem11Index.isValid());

    QPersistentModelIndex treeItem21Index = treeModel->itemToIndex(treeItem21);
    const QPersistentModelIndex tagItem21Index = tagModel->fromSourceIndex(treeItem21Index);
    Q_ASSERT(tagItem21Index.isValid());

    //insert spacer below ti21
    tagModel->addSpacerTag(tagItem21Index, "New Country Name");
 /*
    kDebug()<<"VERIFY IF NEW TAG EXISTS:";
    QModelIndex ti211Spacer = tagModel->index(0,0,ti21);
    Q_ASSERT(ti211Spacer.isValid());
  */

}