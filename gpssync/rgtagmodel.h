/* ============================================================
 *
 * Date        : 2010-03-21
 * Description : A model to hold information about image tags
 *
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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

#ifndef RGTAGMODEL_H
#define RGTAGMODEL_H

// Qt includes

#include <QAbstractItemModel>
#include <QItemSelectionModel>

// local includes

#include "kipiimageitem.h"

namespace KIPIGPSSyncPlugin
{

class RGTagModelPrivate;


class TreeBranch {
public:
    TreeBranch()
    : sourceIndex(),
      parent(0),
      data(),
      type(),
      oldChildren(),
      spacerChildren()
    {
    }

    ~TreeBranch()
    {
        qDeleteAll(oldChildren);
    }

    QPersistentModelIndex sourceIndex;
    TreeBranch* parent;
    QString data;
    Type type;
    QList<TreeBranch*> oldChildren;
    QList<TreeBranch*> spacerChildren;
    QList<TreeBranch*> newChildren;
};



class RGTagModel : public QAbstractItemModel
{
Q_OBJECT

public:
    RGTagModel(QAbstractItemModel* const externalTagModel, QObject* const parent = 0);
    ~RGTagModel();

    // QAbstractItemModel:
    virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    //Local functions:
    QModelIndex fromSourceIndex(const QModelIndex& externalTagModelIndex) const;
    QModelIndex toSourceIndex(const QModelIndex& tagModelIndex) const;
    void addSpacerTag(const QModelIndex&, const QString& );
    QPersistentModelIndex addNewTags(const QModelIndex&, const QString& );
    QList<QList<TagData> > addNewData(QStringList& , QStringList&);
    void addDataInTree(TreeBranch*&, int, QStringList&, QStringList&);
    QList<TagData> getTagAddress();
    void findAndDeleteSpacersOrNewTags(TreeBranch*&, int, const QString& );
    void deleteAllSpacersOrNewTags(const QModelIndex& currentIndex, const QString& whatShouldRemove);
    void readdTag(TreeBranch*&, int, QList<TagData>, int);
    void readdNewTags(QList<QList<TagData> >&);
    void deleteTag(const QModelIndex&);
    QList<QList<TagData> > getSpacers();
    void climbTreeAndGetSpacers(TreeBranch*&); 
    QList<TagData> getSpacerAddress(TreeBranch*);
    void addExternalTags(TreeBranch*, int);
    void addAllExternalTagsToTreeView();
    void addAllSpacersToTag(const QModelIndex, const QStringList, int);
    Type getTagType(const QModelIndex& index) const;
    TreeBranch* branchFromIndex(const QModelIndex& index) const;

public Q_SLOTS:
    void slotSourceDataChanged(const QModelIndex& , const QModelIndex&); 
    void slotSourceHeaderDataChanged(const Qt::Orientation , int, int);
    void slotColumnsAboutToBeInserted ( const QModelIndex &, int , int );     
    void slotColumnsAboutToBeMoved ( const QModelIndex &, int , int , const QModelIndex & , int );
    void slotColumnsAboutToBeRemoved ( const QModelIndex & , int, int  );
    void slotColumnsInserted ();
    void slotColumnsMoved ();
    void slotColumnsRemoved ();
    void slotLayoutAboutToBeChanged ();
    void slotLayoutChanged();
    void slotModelAboutToBeReset();
    void slotModelReset();
    void slotRowsAboutToBeInserted (const QModelIndex &, int , int );
    void slotRowsAboutToBeMoved (const QModelIndex &, int, int, const QModelIndex &, int );
    void slotRowsAboutToBeRemoved (const QModelIndex &, int, int );
    void slotRowsInserted ();
    void slotRowsMoved ();
    void slotRowsRemoved ();

private:
    RGTagModelPrivate* const d;

};


} //KIPIGPSSyncPlugin

#endif
