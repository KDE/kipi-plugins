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


class TreeBranch
{
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

// ----------------------------------------------------------------------------------------

/**
 * @class RGTagModel
 *
 * @brief The model that holds data for the tag tree displayed in ReverseGeocodingWidget
 * 
 * The RGTagModel class is a wrapper above QAbstractItemModel. It helds data for the tag tree displayed in ReverseGeocodingWidget.
 * The model gets the data from the tag model of host application and displays it in a QTreeView. 
 * It stores three type of tags: old tags (the tags that belong to the host's tag model), spacer tags (tags representing address elements
 * or custom tags) and new tags (tags containing data retrieved from backend).
 */
 

class RGTagModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param externalTagModel The tag model found in the host application.
     * @param parent The parent object
     */  
    RGTagModel(QAbstractItemModel* const externalTagModel, QObject* const parent = 0);

    /**
     * Destructor
     */ 
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

    /**
     * Translates the model index from host's tag model to this model.
     * @return The index of current old tag.
     */ 
    QModelIndex fromSourceIndex(const QModelIndex& externalTagModelIndex) const;
    
    /**
     * Translates the model index from this model to host's tag model.
     * @return The index of a tag in host's tag model.
     */ 
    QModelIndex toSourceIndex(const QModelIndex& tagModelIndex) const;
    
    /**
     * Adds a spacer tag.
     * @param parent The index of the parent. If parent == QModelIndex(), then the spacer is added to top-level
     * @param spacerName The name of the spacer. If it's an address element, the address element name will have the form {addressElement}.For example: {Country}, {City}...
     *
     */ 
    void addSpacerTag(const QModelIndex& parent, const QString& spacerName);
    
    /**
     * Adds a tag containing data returned from backends. 
     * @param parent The index of the parent.
     * @param newTagName The name of the new tag.
     */ 
    QPersistentModelIndex addNewTag(const QModelIndex& parent, const QString& newTagName);
    
    /**
     * Add new tags to tag tree. The function starts to scan the tree from root level. When it finds a spacer containing an address element
     * , it looks to see if the address element is found in elements list. If it's found, a new tag is added. 
     * @param elements A list containing address elements. Example: {Country}, {City}...
     * @param resultedData A list containing the name of each address element found in elements. Example: France, Paris...
     * @return A list containing new tags
     */ 
    QList<QList<TagData> > addNewData(QStringList& elements, QStringList& resultedData);
    
    /**
     * The function starts to scan the tree starting with currentBranch. When it finds a spacer containing an address element, it
     * looks to see if the address element is found in addressElements list. If it's found, a new tag is added.
     * @param currentBranch The branch from where the scan starts.
     * @param currentRow The row of the current branch.
     * @param addressElements A list containing address elements. Example: {Country}, {City}...
     * @param elementsData A list containing the name of each address element found in elements. Example: France, Paris...
     */ 
    void addDataInTree(TreeBranch* currentBranch, int currentRow,const QStringList& addressElements,const QStringList& elementsData);
    
    /**
     * Gets the address of a tag.
     */ 
    QList<TagData> getTagAddress();
    
    /**
     * Deletes all spacers or all new tags below @currentBranch.
     * @param currentBranch The tree branch from where the scan starts.
     * @param currentRow The row of current branch.
     * @param whatShouldRemove The tag type that should to be removed. The options are: spacers or new tags.
     */ 
    void findAndDeleteSpacersOrNewTags(TreeBranch* currentBranch, int currentRow, Type whatShouldRemove);
    
    /**
     * Deletes all spacers or all new tags.
     * @param currentIndex If whatShouldRemove represents a spacer, the function will remove all spacers below currentIndex.If whatShouldRemove represents a new tag, the function will delete all new tags.
     * @param whatShouldRemove The tag type that should be removed. The options are: spacers or new tags.
     */ 
    void deleteAllSpacersOrNewTags(const QModelIndex& currentIndex, Type whatShouldRemove);
    
    /**
     * Readds new tags to tag tree.
     * @param currentBranch The branch from where the scan starts.
     * @param currentRow The row of the currentBranch.
     * @param addressElements A list containing address elements. Example: {Country}, {City}...
     * @param elementsData A list containing the name of each address element found in elements. Example: France, Paris...
     */ 
    void readdTag(TreeBranch*& currentBranch, int currentRow,const QList<TagData> tagAddressElements, int currentAddressElementIndex);
    
    /**
     * Takes each tag contained in tagAddressList and adds it to the tag tree.
     * @param tagAddressList A list containing new tags.
     */ 
    void readdNewTags(const QList<QList<TagData> >& tagAddressList);
    
    /**
     * Deletes a tag.
     * @param currentIndex The tag found at this index will be deleted.
     */ 
    void deleteTag(const QModelIndex& currentIndex);
    
    /**
     * Gets all spacers.
     * @return The spacer list.
     */ 
    QList<QList<TagData> > getSpacers();
    
    /**
     * Gets the spacers addresses below currentBranch. Address means the path from rootTag to currentBranch.
     * @param currentBranch The branch from where the search starts. 
     */ 
    void climbTreeAndGetSpacers(const TreeBranch* currentBranch);
    
    /**
     * Gets the address of a spacer. Address means the path from rootTag to currentBranch
     * @param currentBranch The branch where the scan stops.
     * @return The tag address of currentBranch
     */ 
    QList<TagData> getSpacerAddress(TreeBranch* currentBranch);
    
    /**
     * Add tags from host application to the tag tree.
     * @param parentBranch The branch that will be parent for the old tag.
     * @param currentRow The row where this external tag will be added.
     */ 
    void addExternalTags(TreeBranch* parentBranch, int currentRow);
    
    /**
     * Add all external tags to the tag tree.
     */ 
    void addAllExternalTagsToTreeView();
    
    /**
     * Adds all spacers found in spacerList to the tag tree. 
     */ 
    void addAllSpacersToTag(const QModelIndex currentIndex, const QStringList spacerList, int spacerListIndex);
    
    /**
     * Gets the type of a tag found at index.
     * @param index The index of the tag.
     * @return The type of the tag found at index.
     */ 
    Type getTagType(const QModelIndex& index) const;
    
    /**
     * Returns the branch found at index
     * @param index Current model index.
     * @return The branch for the current index.
     */ 
    TreeBranch* branchFromIndex(const QModelIndex& index) const;

public Q_SLOTS:

    void slotSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void slotSourceHeaderDataChanged(const Qt::Orientation orientation, int first, int last);
    void slotColumnsAboutToBeInserted ( const QModelIndex & parent, int start, int end);
    void slotColumnsAboutToBeMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn);
    void slotColumnsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );
    void slotColumnsInserted ();
    void slotColumnsMoved ();
    void slotColumnsRemoved ();
    void slotLayoutAboutToBeChanged ();
    void slotLayoutChanged();
    void slotModelAboutToBeReset();
    void slotModelReset();
    void slotRowsAboutToBeInserted (const QModelIndex & parent, int start, int end);
    void slotRowsAboutToBeMoved (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    void slotRowsAboutToBeRemoved (const QModelIndex & parent, int start, int end);
    void slotRowsInserted ();
    void slotRowsMoved ();
    void slotRowsRemoved ();

private:

    RGTagModelPrivate* const d;
};

} // namespace KIPIGPSSyncPlugin

#endif // RGTAGMODEL_H
