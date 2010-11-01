/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-06-21
 * @brief  Test for SimpleTreeModel.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#include "test_simpletreemodel.moc"

// KDE includes

#include <qtest_kde.h>
#include <kdebug.h>
#include <kurl.h>

// local includes

#include "simpletreemodel.h"
#include "../../borrowed/modeltest.h"

QTEST_KDEMAIN_CORE(TestSimpleTreeModel)

/**
 * @brief Dummy test that does nothing
 */
void TestSimpleTreeModel::testNoOp()
{
}

void TestSimpleTreeModel::testModel1()
{
    SimpleTreeModel* const treeModel = new SimpleTreeModel(1, this);
    new ModelTest(treeModel, this);
    Q_ASSERT(!treeModel->index(0, 0).isValid());
    Q_ASSERT(treeModel->indexToItem(QModelIndex())==treeModel->rootItem());
    Q_ASSERT(!treeModel->itemToIndex(treeModel->rootItem()).isValid());
    Q_ASSERT(!treeModel->itemToIndex(0).isValid());
    Q_ASSERT(!treeModel->parent(QModelIndex()).isValid());

    SimpleTreeModel::Item* const item1 = treeModel->addItem();
    Q_ASSERT(item1!=0);
    const QPersistentModelIndex item1Index = treeModel->itemToIndex(item1);
    Q_ASSERT(item1Index.isValid());
    Q_ASSERT(treeModel->indexToItem(item1Index)==item1);
    Q_ASSERT(!treeModel->parent(item1Index).isValid());

    SimpleTreeModel::Item* const item2 = treeModel->addItem();
    Q_ASSERT(item2!=0);
    const QModelIndex item2Index = treeModel->itemToIndex(item2);
    Q_ASSERT(item2Index.isValid());
    Q_ASSERT(treeModel->indexToItem(item2Index)==item2);
    Q_ASSERT(!treeModel->parent(item2Index).isValid());

    SimpleTreeModel::Item* const item21 = treeModel->addItem(item2);
    Q_ASSERT(item21!=0);
    const QModelIndex item21Index = treeModel->itemToIndex(item21);
    Q_ASSERT(item21Index.isValid());
    Q_ASSERT(treeModel->indexToItem(item21Index)==item21);
    Q_ASSERT(treeModel->parent(item21Index)==item2Index);
    Q_ASSERT(treeModel->index(0, 0, item2Index)==item21Index);

    // just make sure another modeltest will test things for consistency in case a signal went missing
    new ModelTest(treeModel, this);

    Q_ASSERT(treeModel->rootItem() == treeModel->indexToItem(QModelIndex()));
    Q_ASSERT(treeModel->indexToItem(treeModel->itemToIndex(item1))==item1);
    Q_ASSERT(treeModel->hasIndex(0, 0) == true);

    QModelIndex topIndex = treeModel->index(0, 0, QModelIndex());
    if (treeModel->rowCount(topIndex) > 0) {
        QModelIndex childIndex = treeModel->index(0, 0, topIndex);
        kDebug()<<childIndex;
        kDebug()<<treeModel->parent(childIndex);
        Q_ASSERT(treeModel->parent(childIndex) == topIndex);
    }

    // add another few items:
    {
        SimpleTreeModel::Item* const item21 = treeModel->addItem(item2, 0);
        Q_ASSERT(item21!=0);
        const QModelIndex item21Index = treeModel->itemToIndex(item21);
        Q_ASSERT(item21Index.isValid());
        Q_ASSERT(treeModel->indexToItem(item21Index)==item21);
        Q_ASSERT(treeModel->parent(item21Index)==item2Index);
        Q_ASSERT(treeModel->index(0, 0, item2Index)==item21Index);
        Q_ASSERT(item21Index.row()==0);
    }

    // add another few items:
    {
        SimpleTreeModel::Item* const item21 = treeModel->addItem(item2, 1);
        Q_ASSERT(item21!=0);
        const QModelIndex item21Index = treeModel->itemToIndex(item21);
        Q_ASSERT(item21Index.isValid());
        Q_ASSERT(treeModel->indexToItem(item21Index)==item21);
        Q_ASSERT(treeModel->parent(item21Index)==item2Index);
        Q_ASSERT(treeModel->index(1, 0, item2Index)==item21Index);
        Q_ASSERT(item21Index.row()==1);
    }

    new ModelTest(treeModel, this);
}
