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
    SimpleTreeModel* const treeModel = new SimpleTreeModel(this);
    new ModelTest(treeModel, this);

    KIPIGPSSyncPlugin::RGTagModel* const tagModel = new KIPIGPSSyncPlugin::RGTagModel(treeModel, this);
    new ModelTest(tagModel, this);
}

void TestRGTagModel::testModel1()
{
    SimpleTreeModel* const treeModel = new SimpleTreeModel(this);
    new ModelTest(treeModel, this);

    // add some items before the tagModel is created:
    SimpleTreeModel::Item* const treeItem1 = treeModel->addItem();
    QPersistentModelIndex treeItem1Index = treeModel->itemToIndex(treeItem1);
    SimpleTreeModel::Item* const treeItem11 = treeModel->addItem(treeItem1);
    QPersistentModelIndex treeItem11Index = treeModel->itemToIndex(treeItem11);

    KIPIGPSSyncPlugin::RGTagModel* const tagModel = new KIPIGPSSyncPlugin::RGTagModel(treeModel, this);
    // TODO: make sure the ModelTest does not find any errors, currently it does find errors ;-)
//     new ModelTest(tagModel, this);

    // simple tests
    Q_ASSERT(tagModel->rowCount()==treeModel->rowCount());
    const QPersistentModelIndex tagItem1Index = tagModel->fromSourceIndex(treeItem1Index);
    Q_ASSERT(tagItem1Index.isValid());
    Q_ASSERT(tagModel->rowCount(tagItem1Index)==treeModel->rowCount(treeItem1Index));

    // make sure the tagModel handles items inserted after it was created
    // - both top level
    SimpleTreeModel::Item* const treeItem2 = treeModel->addItem();
    QPersistentModelIndex treeItem2Index = treeModel->itemToIndex(treeItem2);
    Q_ASSERT(tagModel->rowCount()==treeModel->rowCount());

    // - and sub items:
    SimpleTreeModel::Item* const treeItem21 = treeModel->addItem(treeItem2);
    const QPersistentModelIndex tagItem2Index = tagModel->fromSourceIndex(treeItem2Index);
    Q_ASSERT(tagItem2Index.isValid());
    Q_ASSERT(tagModel->rowCount(tagItem2Index)==treeModel->rowCount(treeItem2Index));
}
