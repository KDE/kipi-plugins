
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
    // type = 0 => TypeChild
    // type = 1 => TypeSpacer
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
        kDebug()<<"Index children:"<<checkBranch->sourceIndex<<"LEVEL:"<<level+1;
        checkTree(checkBranch->children[j], level+1);  
    }

    for(int j = 0; j < checkBranch->spacerChildren.count(); ++j)
    {
        kDebug()<<"Index spacer:"<<checkBranch->sourceIndex<<"LEVEL:"<<level+1;
        checkTree(checkBranch->spacerChildren[j], level+1);
    }
}

QModelIndex RGTagModel::fromSourceIndex(const QModelIndex& externalTagModelIndex) const
{
    if(!externalTagModelIndex.isValid())
        return QModelIndex();

    //QModelIndex translatedExternalIndex = createIndex(externalTagModelIndex.row

    QList<QModelIndex> parents;
    QModelIndex myIndex = externalTagModelIndex;
    parents<<myIndex;
    while(myIndex.parent().isValid())
    {
        myIndex = myIndex.parent();
        parents.prepend(myIndex);
    }
    //parents.prepend(QModelIndex());
    kDebug()<<"Parents:"<<parents;

    TreeBranch* subModelBranch = d->rootTag;

    int level = 0;
    while(level <= parents.count())
    {

        if(subModelBranch->sourceIndex == externalTagModelIndex)
        {
            kDebug()<<"Index root:"<<d->rootTag->sourceIndex<<"LEVEL:0";
        
            for (int j=0; j<d->rootTag->children.count(); ++j)
            {
                kDebug()<<"Index children:"<<d->rootTag->children[j]->sourceIndex<<"LEVEL:1";
                checkTree(d->rootTag->children[j], 1);
            }

            for (int j=0; j<d->rootTag->spacerChildren.count();++j)
            {
                kDebug()<<"Index spacer:"<<d->rootTag->spacerChildren[j]->sourceIndex<<"LEVEL:1";
                checkTree(d->rootTag->spacerChildren[j], 1);
            }

            return createIndex(subModelBranch->sourceIndex.row()+subModelBranch->parent->spacerChildren.count(), subModelBranch->sourceIndex.column(), subModelBranch);

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
    kDebug()<<"Entered toSourceIndex";
    if(!tagModelIndex.isValid())
        return QModelIndex();

    TreeBranch* const treeBranch = static_cast<TreeBranch*>(tagModelIndex.internalPointer());
    if(!treeBranch)
        return QModelIndex();

    kDebug()<<"Exists toSourceIndex";
    return treeBranch->sourceIndex;
}

void RGTagModel::addSpacerTag(const QModelIndex& parent, const QString& spacerName)
{
    
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(parent.internalPointer());

    TreeBranch* newSpacer = new TreeBranch();
    newSpacer->parent = parentBranch;
    newSpacer->data = spacerName;
    newSpacer->type = TypeChild;
    beginInsertRows(parent, 0, 0); 
    parentBranch->spacerChildren.append(newSpacer);
    endInsertRows();
}

int RGTagModel::columnCount(const QModelIndex& parent) const
{
    kDebug()<<"Entered column count";
    //Change something here?
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(parent.internalPointer());
    if(!parentBranch)
    {
        return 0;
    }    

    if(parentBranch && parentBranch->type == TypeSpacer)
    {
        return 1;
    }
    
    return d->tagModel->columnCount(toSourceIndex(parent));

}

bool RGTagModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

QVariant RGTagModel::data(const QModelIndex& index, int role) const
{
    kDebug()<<"Entered data function"; 
    TreeBranch* const treeBranch = static_cast<TreeBranch*>(index.internalPointer());
    if((!treeBranch) || (treeBranch->type != TypeSpacer)) 
        return d->tagModel->data(toSourceIndex(index), role);
    else if(role == Qt::DisplayRole)
    {
        kDebug()<<"Exists data function";
        return treeBranch->data;
    }
    else
    {
        kDebug()<<"Exists data function";
        return QVariant();
    } 
}

QModelIndex RGTagModel::index(int row, int column, const QModelIndex& parent) const
{

    if ( (column!=0) || (row<0) )
        return QModelIndex();

    TreeBranch* parentBranch = d->rootTag;
    if (parent.isValid())
        parentBranch = static_cast<TreeBranch*>(parent.internalPointer());

    kDebug()<<row<<column<<parent<<parentBranch;

    // this should not happen!
    if (!parentBranch)
        return QModelIndex();

    kDebug()<<"parentBranch->sourceIndex="<<parentBranch->sourceIndex;
    kDebug()<<"parentBranch->children.count()="<<parentBranch->children.count();

    if (row < parentBranch->spacerChildren.count())
    {
        return createIndex(row, column, parentBranch->spacerChildren[row]);
    } else {
        return fromSourceIndex(d->tagModel->index(row-parentBranch->spacerChildren.count(),column,toSourceIndex(parent)));
    }

    return QModelIndex();
}

QModelIndex RGTagModel::parent(const QModelIndex& index) const
{
/*
    kDebug()<<"Entered parent";

    TreeBranch* const currentBranch = static_cast<TreeBranch*>(index.internalPointer());
    if(currentBranch && (currentBranch->type == TypeSpacer))
    {
        TreeBranch* const parentBranch = currentBranch->parent;
        if(parentBranch)
        {
                TreeBranch* const gParentBranch = parentBranch->parent;
                if(gParentBranch)
                {
                    int parentRow = gParentBranch->children.count();
                    int found = 0;
                    for( int i=0; i<gParentBranch->spacerChildren.count(); ++i)
                    {
                        //I'm not sure if == is a good comparision. 
                        if(gParentBranch->spacerChildren[i] == parentBranch)
                        {
                            found = 1;
                            break;
                        }
                        parentRow++;
                    }
                    if(found == 1)
                    {
                        kDebug()<<"Exists parent";
                        return createIndex(parentRow, 0, parentBranch);
                    }
                    else
                    {
                        kDebug()<<"Exists parent";
                        return QModelIndex();
                    }
                }
        }
    }
    kDebug()<<"Exists parent";*/
    return fromSourceIndex(d->tagModel->parent(toSourceIndex(index)));
}

int RGTagModel::rowCount(const QModelIndex& parent) const
{
    kDebug()<<"Entered rowCount";
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
    kDebug()<<"Entered headerData";
    return d->tagModel->headerData(section, orientation, role);
}

Qt::ItemFlags RGTagModel::flags(const QModelIndex& index) const
{
    kDebug()<<"Entered flags";
    TreeBranch* const currentBranch = static_cast<TreeBranch*>(index.internalPointer());
    if(currentBranch && currentBranch->type == TypeSpacer)
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
    kDebug()<<"Entered columnsAboutToBeInserted";
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
/*    kDebug()<<"Entered rowsAboutToBeInserted";
    kDebug()<<"Start:"<<start<<" End:"<<end; 
    TreeBranch* const parentBranch = static_cast<TreeBranch*>(fromSourceIndex(parent).internalPointer());
    if(parentBranch)
        kDebug()<<"children.count()="<<parentBranch->children.count();
    if(parentBranch && start > parentBranch->children.count())
    {
        kDebug()<<"Entered spacer if";
        beginInsertRows(parent, start, end);
        kDebug()<<"Exists rowsAboutToBeInserted through spacer if";
    }
    else 
    {*/
   //     kDebug()<<"Exists rowsAboutToBeInserted through tag model";
        beginInsertRows(fromSourceIndex(parent), start, end);
  //  }
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
    kDebug()<<"Entered rowsInserted";
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
