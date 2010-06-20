
#include "rgtagmodel.moc"

#include "rgtagmodel.h"
#include "kdebug.h"


namespace KIPIGPSSyncPlugin
{

class TreeBranch {
public:
 QPersistentModelIndex sourceIndex;
 TreeBranch* parent;
 QList<TreeBranch*> children;
};


class RGTagModelPrivate
{
public:
    
    RGTagModelPrivate()
    : tagModel()
    {
    }

    QAbstractItemModel* tagModel;
    QList<TreeBranch*> tagTree;
};

RGTagModel::RGTagModel(QAbstractItemModel* tagModel, QObject* const parent)
: QAbstractItemModel(parent), d(new RGTagModelPrivate)
{
    d->tagModel = tagModel;
   
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
    delete d;
}


QModelIndex RGTagModel::fromSourceIndex(const QModelIndex& externalTagModelIndex) const
{

    kDebug()<<"Entered in fromSourceIndex";
    kDebug()<<"Current Index:"<<externalTagModelIndex;
    kDebug()<<"Parent index:"<<externalTagModelIndex.parent();
    kDebug()<<"Parent parent index:"<<externalTagModelIndex.parent().parent();
    kDebug()<<"Parent parent parent index:"<<externalTagModelIndex.parent().parent().parent();

    QList<QModelIndex> parents;
    QModelIndex myIndex = externalTagModelIndex;
    parents<<myIndex;
    while(myIndex.isValid())
    {
        kDebug()<<"Entered while:"<<myIndex;
        parents.prepend(myIndex.parent());
        myIndex = myIndex.parent();
        kDebug()<<"Exists while:"<<myIndex;
    }
    parents.prepend(QModelIndex());

    kDebug()<<"Exists while good!";

    TreeBranch* subModelBranch;
    if(d->tagTree.count() == 0)
    {
        subModelBranch = new TreeBranch();
        subModelBranch->parent = NULL;
        subModelBranch->sourceIndex = parents[0];
    
        d->tagTree.append(subModelBranch);
    }
    else
    {
        subModelBranch = d->tagTree.first();
    }

    kDebug()<<"Allocates subModelBranch good!";

    int found,where;
    int level = 0;
    while((subModelBranch->sourceIndex != externalTagModelIndex) || (level != parents.count()))
    {

        kDebug()<<"Enters in second while"<<subModelBranch;

        if(subModelBranch->sourceIndex == externalTagModelIndex)
        {
            return createIndex(subModelBranch->sourceIndex.row(), subModelBranch->sourceIndex.column(), subModelBranch);
        }

        kDebug()<<"Passes the if";

        where = -1;
        for( int i=0; i < subModelBranch->children.count(); ++i)
        {
            if(subModelBranch->children[i]->sourceIndex == parents[level])
            {
                where = i;
                break;
            }
        }        

        kDebug()<<"Passes the for.Where:"<<where;

        if(where >= 0)
        {
            subModelBranch = subModelBranch->children[where];
        }
        else
        {
            kDebug()<<"Parents:"<<parents;
            //TODO: check when rows are different
            TreeBranch* newTreeBranch = new TreeBranch();
            newTreeBranch->sourceIndex = parents[level+1];
            kDebug()<<"Passes newTreeBranch->sourceIndex = parents[level+1]";
            kDebug()<<"Parent:"<<parents[level]<<" Child:"<<parents[level+1];
            newTreeBranch->parent = subModelBranch;
            kDebug()<<"Passes newTreeBranch->parent->sourceIndex = parents[level]";
            
            
            kDebug()<<"Passes the creation of newTreeBranch"; 

            subModelBranch->children.append(newTreeBranch); 
            kDebug()<<"Passes the append of newTreeBranch to subModelBranch)";  
            subModelBranch = subModelBranch->children.last();    
            kDebug()<<"passes subModelBranch=subModelBranch->children"; 
        }
        level++;
        kDebug()<<"Passes "<<level<<" iteration in second while";

    }

    //return createIndex(externalTagModelIndex.row(), externalTagModelIndex.column());
}

QModelIndex RGTagModel::toSourceIndex(const QModelIndex& tagModelIndex) const
{
    return createIndex(tagModelIndex.row(), tagModelIndex.column());
}

int RGTagModel::columnCount(const QModelIndex& parent) const
{
    kDebug()<<"Entered in columnCount";
    return d->tagModel->columnCount(fromSourceIndex(parent));

}
bool RGTagModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return true;
}

QVariant RGTagModel::data(const QModelIndex& index, int role) const
{

    kDebug()<<"Entered in data";
    return d->tagModel->data(fromSourceIndex(index), role);
    
    //return d->tagModel->data(
}

QModelIndex RGTagModel::index(int row, int column, const QModelIndex& parent) const
{
   
    kDebug()<<"Entered in index";
    return d->tagModel->index(row,column,fromSourceIndex(parent));
}

QModelIndex RGTagModel::parent(const QModelIndex& index) const
{
    kDebug()<<"Entered in parent";
    return d->tagModel->parent(fromSourceIndex(index));
}

int RGTagModel::rowCount(const QModelIndex& parent) const
{
    kDebug()<<"Entered in rowCount";
    return d->tagModel->rowCount(fromSourceIndex(parent));
}

bool RGTagModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    kDebug()<<"Entered in setHeaderData";
    return true;
}

QVariant RGTagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    kDebug()<<"Entered in headerData";
    return d->tagModel->headerData(section, orientation, role);
}

Qt::ItemFlags RGTagModel::flags(const QModelIndex& index) const
{
    kDebug()<<"Entered in flags";
    return d->tagModel->flags(fromSourceIndex(index));
}

void RGTagModel::slotSourceDataChanged(const QModelIndex& row, const QModelIndex& column)
{
    kDebug()<<"Entered in slotSourceDataChanged";
    emit dataChanged(row,column);

}

void RGTagModel::slotSourceHeaderDataChanged(const Qt::Orientation orientation, int first, int last)
{
    kDebug()<<"Entered in slotSourceHeaderDataChanged";
    emit headerDataChanged(orientation, first, last); 
}

void RGTagModel::slotColumnsAboutToBeInserted ( const QModelIndex & parent, int start, int end )
{
    kDebug()<<"Entered in slotColumnsAboutToBeInserted";
   beginInsertColumns (fromSourceIndex(parent), start, end); 

}
void RGTagModel::slotColumnsAboutToBeMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn )
{
    kDebug()<<"Entered in slotColumnsAboutToBeMoved";
    beginMoveColumns( fromSourceIndex(sourceParent), sourceStart, sourceEnd, fromSourceIndex(destinationParent), destinationColumn );
}
void RGTagModel::slotColumnsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{
    kDebug()<<"Entered in slotColumnsAboutToBeRemoved";
    beginRemoveColumns ( fromSourceIndex(parent), start, end);
}
void RGTagModel::slotColumnsInserted ()
{
    kDebug()<<"Entered in slotColumnsInserted";
    endInsertColumns();
}
void RGTagModel::slotColumnsMoved ()
{
    kDebug()<<"Entered in slotColumnsMoved";
    endMoveColumns();
}
void RGTagModel::slotColumnsRemoved ()
{
    kDebug()<<"Entered in slotColumnsRemoved";
    endRemoveColumns();
}
void RGTagModel::slotLayoutAboutToBeChanged ()
{
    kDebug()<<"Entered in slotLayoutAboutToBeChanged";
    emit layoutAboutToBeChanged();
}
void RGTagModel::slotLayoutChanged()
{
    kDebug()<<"Entered in slotLayoutChanged";
    emit layoutChanged();
}
void RGTagModel::slotModelAboutToBeReset()
{
    kDebug()<<"Entered in slotModelAboutToBeReset";
    beginResetModel();
}
void RGTagModel::slotModelReset()
{
    kDebug()<<"Entered in slotModelReset";
    
    reset();
}
void RGTagModel::slotRowsAboutToBeInserted (const QModelIndex & parent, int start, int end )
{
    kDebug()<<"Entered in slotRowsAboutToBeInserted";
    beginInsertRows(fromSourceIndex(parent), start, end);
}
void RGTagModel::slotRowsAboutToBeMoved (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow )
{
    kDebug()<<"Entered in slowRowsAboutToBeMoved";
    beginMoveRows( fromSourceIndex(sourceParent), sourceStart, sourceEnd, fromSourceIndex(destinationParent), destinationRow );
}
void RGTagModel::slotRowsAboutToBeRemoved (const QModelIndex & parent, int start, int end )
{
    kDebug()<<"Entered in slotRowsAboutToBeRemoved";
    beginRemoveRows(fromSourceIndex(parent), start, end);
}
void RGTagModel::slotRowsInserted ()
{
    kDebug()<<"Entered in slotRowsInserted";
    endInsertRows();
}
void RGTagModel::slotRowsMoved ()
{
    kDebug()<<"Entered in slotRowsMoved";
    endMoveRows();
}
void RGTagModel::slotRowsRemoved ()
{
    kDebug()<<"Entered in slotRowsRemoved";
    endRemoveRows();
}


}    //KIPIGPSSyncPlugin
