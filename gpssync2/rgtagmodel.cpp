
#include "rgtagmodel.moc"

#include "rgtagmodel.h"
#include "kdebug.h"


namespace KIPIGPSSyncPlugin
{

class TreeBranch {
public:
    TreeBranch()
    : sourceIndex(),
      parent(0),
      children(),
      type(0),
      spacerChildren()
    {
    }

    ~TreeBranch()
    {
        qDeleteAll(children);
    }

    QPersistentModelIndex sourceIndex;
    TreeBranch* parent;
    QString data;
    // type = 0 => is old tag
    // type = 1 => is spacer
    int type;
    QModelIndex internalIndex; 
    QList<TreeBranch*> children;
    QList<TreeBranch*> spacerChildren;
};


class RGTagModelPrivate
{
public:
    RGTagModelPrivate()
    : tagModel(),
      rootTag(0)
    {
    }

    QAbstractItemModel* tagModel;
    TreeBranch* rootTag;
};

RGTagModel::RGTagModel(QAbstractItemModel* const externalTagModel, QObject* const parent)
: QAbstractItemModel(parent), d(new RGTagModelPrivate)
{
    d->tagModel = externalTagModel;
    d->rootTag = new TreeBranch();
   
    connect(d->tagModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotSourceDataChanged(const QModelIndex&, const QModelIndex&))); 

    connect(d->tagModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
            this, SLOT(slotSourceHeaderDataChanged(Qt::Orientation, int, int)));
    connect(d->tagModel, SIGNAL(columnsAboutToBeInserted(const QModelIndex&, int, int)),
            this, SLOT(slotColumnsAboutToBeInserted(const QModelIndex&, int, int)));
    connect(d->tagModel, SIGNAL(columnsAboutToBeMoved(const QModelIndex&, int,int,const QModelIndex&, int)),
            this, SLOT(slotColumnsAboutToBeMoved(const QModelIndex&,int,int,const QModelIndex&, int)));
    connect(d->tagModel, SIGNAL(columnsAboutToBeRemoved(const QModelIndex&, int, int)),
            this, SLOT(slotColumnsAboutToBeRemoved(const QModelIndex&, int, int))); 
    connect(d->tagModel, SIGNAL(columnsInserted(const QModelIndex&, int, int)),
            this, SLOT(slotColumnsInserted()));
    connect(d->tagModel, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)), 
            this, SLOT(slotColumnsMoved()));
    connect(d->tagModel, SIGNAL(columnsRemoved(const QModelIndex &, int, int)), 
            this, SLOT(slotColumnsRemoved()));
    connect(d->tagModel, SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(slotLayoutAboutToBeChanged()));
    connect(d->tagModel, SIGNAL(layoutChanged()),
            this, SLOT(slotLayoutChanged()));
    connect(d->tagModel, SIGNAL(modelAboutToBeReset()),
            this, SLOT(slotModelAboutToBeReset()));
    connect(d->tagModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
            this, SLOT(slotRowsAboutToBeInserted(const QModelIndex&, int, int)));

    connect(d->tagModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int,int,const QModelIndex&, int)),
            this, SLOT(slotRowsAboutToBeMoved(const QModelIndex&,int,int,const QModelIndex&, int)));
    connect(d->tagModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
            this, SLOT(slotRowsAboutToBeRemoved(const QModelIndex&, int, int)));
    connect(d->tagModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(slotRowsInserted()));
    connect(d->tagModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SLOT(slotRowsMoved()));
    connect(d->tagModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SLOT(slotRowsRemoved()));
}

RGTagModel::~RGTagModel()
{
    delete d->rootTag;

    delete d;
}

void checkTree(TreeBranch* const checkBranch, int level)
{
    if(!checkBranch->sourceIndex.isValid())
        return;

    for(int j = 0; j < checkBranch->children.count(); ++j)
    {
        checkTree(checkBranch->children[j], level+1);  
    }
}

QModelIndex RGTagModel::fromSourceIndex(const QModelIndex& externalTagModelIndex) const
{
    if(!externalTagModelIndex.isValid())
        return QModelIndex();

    QList<QModelIndex> parents;
    QModelIndex myIndex = externalTagModelIndex;
    parents<<myIndex;
    while(myIndex.parent().isValid())
    {
        myIndex = myIndex.parent();
        parents.prepend(myIndex);
    }

    TreeBranch* subModelBranch = d->rootTag;

    int level = 0;
    while(level <= parents.count())
    {

        if(subModelBranch->sourceIndex == externalTagModelIndex)
        {
            for (int j=0; j<d->rootTag->children.count(); ++j)
            {
                checkTree(d->rootTag->children[j], 2);
            }

            return createIndex(subModelBranch->sourceIndex.row(), subModelBranch->sourceIndex.column(), subModelBranch);
            //return subModelBranch->internalIndex;

        }

        int where = -1;
        for (int i=0; i < subModelBranch->children.count(); ++i)
        {
            if(subModelBranch->children[i]->sourceIndex == parents[level])
            {
                where = i;
                break;
            }
        }        


        if(where >= 0)
        {
            subModelBranch = subModelBranch->children[where];
        }
        else
        {
            if (level>=parents.count())
                return QModelIndex();

            //TODO: check when rows are different
            TreeBranch* newTreeBranch = new TreeBranch();
            newTreeBranch->sourceIndex = parents[level];
            newTreeBranch->parent = subModelBranch;

            subModelBranch->children.append(newTreeBranch); 
            subModelBranch = newTreeBranch;    
        }
        level++;

    }

    //no index is found
    return QModelIndex();
}

QModelIndex RGTagModel::toSourceIndex(const QModelIndex& tagModelIndex) const
{
    if(!tagModelIndex.isValid())
        return QModelIndex();

    TreeBranch* const treeBranch = static_cast<TreeBranch*>(tagModelIndex.internalPointer());
    if(!treeBranch)
        return QModelIndex();

    return treeBranch->sourceIndex;
}

void RGTagModel::addSpacerTag(QModelIndex& parent, QString spacerName)
{
    
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(parent.internalPointer());

    TreeBranch* newSpacer = new TreeBranch();
    newSpacer->parent = parentBranch;
    newSpacer->data = spacerName;
    newSpacer->type = 1;
    
    parentBranch->spacerChildren.append(newSpacer);

}

int RGTagModel::columnCount(const QModelIndex& parent) const
{
    
    return d->tagModel->columnCount(toSourceIndex(parent));

}

bool RGTagModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

QVariant RGTagModel::data(const QModelIndex& index, int role) const
{
    
    TreeBranch* const treeBranch = static_cast<TreeBranch*>(index.internalPointer());
    if((!treeBranch) || (treeBranch->type != 1))
        return d->tagModel->data(toSourceIndex(index), role);
    else if(role == Qt::DisplayRole)
    {
        return treeBranch->data;
    }
    else
        return QVariant();
}

QModelIndex RGTagModel::index(int row, int column, const QModelIndex& parent) const
{
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(parent.internalPointer());
    
    if(parentBranch)
    {
        //here are some problems
        if(row > parentBranch->children.count() && (row != 0))
        {
            
            return createIndex( row, column, parentBranch->spacerChildren[row-parentBranch->children.count()]);
        }
    }
    
    return fromSourceIndex(d->tagModel->index(row,column,toSourceIndex(parent)));
}

QModelIndex RGTagModel::parent(const QModelIndex& index) const
{

    TreeBranch* const currentBranch = static_cast<TreeBranch*>(index.internalPointer());
    if(currentBranch && (currentBranch->type == 1))
    {
        TreeBranch* const parentBranch = currentBranch->parent;
        if(parentBranch)
        {
            if(parentBranch->type == 1)
            {
                TreeBranch* const gParentBranch = parentBranch->parent;
                if(gParentBranch)
                {
                    int parentRow = gParentBranch->children.count();
                    for( int i=0; i<gParentBranch->spacerChildren.count(); ++i)
                    {
                        //I'm not sure if == is a good comparision. 
                        if(gParentBranch->spacerChildren[i] == parentBranch)
                        {
                            break;
                        }
                        parentRow++;
                    }
                    return createIndex(parentRow, 0, parentBranch);
                }
            }
        }
    }

    return fromSourceIndex(d->tagModel->parent(toSourceIndex(index)));
}

int RGTagModel::rowCount(const QModelIndex& parent) const
{
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(parent.internalPointer());
    
    if(!parentBranch)
        return d->tagModel->rowCount(toSourceIndex(parent));
    else
        return  parentBranch->spacerChildren.count() + d->tagModel->rowCount(toSourceIndex(parent));
}

bool RGTagModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    return false;
}

QVariant RGTagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->tagModel->headerData(section, orientation, role);
}

Qt::ItemFlags RGTagModel::flags(const QModelIndex& index) const
{
    TreeBranch* const currentBranch = static_cast<TreeBranch*>(index.internalPointer());
    if(currentBranch && currentBranch->type == 1)
    {
        return QAbstractItemModel::flags(index);
    }    

    return d->tagModel->flags(toSourceIndex(index));
}

void RGTagModel::slotSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    emit dataChanged(fromSourceIndex(topLeft),fromSourceIndex(bottomRight));

}

void RGTagModel::slotSourceHeaderDataChanged(const Qt::Orientation orientation, int first, int last)
{
    emit headerDataChanged(orientation, first, last); 
}

void RGTagModel::slotColumnsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    //TODO: Should we do something here?
    beginInsertColumns(fromSourceIndex(parent), start, end);
}

void RGTagModel::slotColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationColumn)
{
    beginMoveColumns(fromSourceIndex(sourceParent), sourceStart, sourceEnd, fromSourceIndex(destinationParent), destinationColumn );
}

void RGTagModel::slotColumnsAboutToBeRemoved(const QModelIndex& parent, int start, int end )
{
    beginRemoveColumns(fromSourceIndex(parent), start, end);
}

void RGTagModel::slotColumnsInserted()
{
    endInsertColumns();
}

void RGTagModel::slotColumnsMoved()
{
    endMoveColumns();
}

void RGTagModel::slotColumnsRemoved()
{
    endRemoveColumns();
}

void RGTagModel::slotLayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void RGTagModel::slotLayoutChanged()
{
    emit layoutChanged();
}

void RGTagModel::slotModelAboutToBeReset()
{
    beginResetModel();
}

void RGTagModel::slotModelReset()
{
    reset();
}

void RGTagModel::slotRowsAboutToBeInserted(const QModelIndex& parent, int start, int end )
{
    
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(parent.internalPointer());
    if(parentBranch && start >= parentBranch->children.count())
    {
        beginInsertRows(parent, start, end);
    }

    beginInsertRows(fromSourceIndex(parent), start, end);
}

void RGTagModel::slotRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow)
{
    beginMoveRows(fromSourceIndex(sourceParent), sourceStart, sourceEnd, fromSourceIndex(destinationParent), destinationRow );
}

void RGTagModel::slotRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    beginRemoveRows(fromSourceIndex(parent), start, end);
}

void RGTagModel::slotRowsInserted()
{
    endInsertRows();
}

void RGTagModel::slotRowsMoved()
{
    endMoveRows();
}

void RGTagModel::slotRowsRemoved()
{
    endRemoveRows();
}

}    //KIPIGPSSyncPlugin
