
#include "rgtagmodel.moc"

#include "rgtagmodel.h"
#include "kdebug.h"


namespace KIPIGPSSyncPlugin
{

class RGTagModelPrivate
{
public:
    
    RGTagModelPrivate()
    : tagModel()
    {
    }

    QAbstractItemModel* tagModel;

};

RGTagModel::RGTagModel(QAbstractItemModel* tagModel, QObject* const parent)
: QAbstractItemModel(parent), d(new RGTagModelPrivate)
{
    d->tagModel = tagModel;
   
    connect(d->tagModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotSourceDataChanged(const QModelIndex&, const QModelIndex&))); 

    connect(d->tagModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
            this, SLOT(slotSourceHeaderDataChanged(Qt::Orientation, int int)));
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
            this, SLOT(RowsInserted()));
    connect(d->tagModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SLOT(slotRowsMoved()));
    connect(d->tagModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SLOT(slotRowsRemoved()));

    

}

RGTagModel::~RGTagModel()
{
    delete d;
}

int RGTagModel::columnCount(const QModelIndex& parent) const
{
    return d->tagModel->columnCount(parent);

}
bool RGTagModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    d->tagModel->setData(index, value, role);
    return true;
}

QVariant RGTagModel::data(const QModelIndex& index, int role) const
{

    return d->tagModel->data(index, role);

}

QModelIndex RGTagModel::index(int row, int column, const QModelIndex& parent) const
{
    return d->tagModel->index(row,column,parent);
}

QModelIndex RGTagModel::parent(const QModelIndex& index) const
{
    return d->tagModel->parent(index);
}

int RGTagModel::rowCount(const QModelIndex& parent) const
{
    return d->tagModel->rowCount(parent);
}

bool RGTagModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    return true;
}

QVariant RGTagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->tagModel->headerData(section, orientation, role);
}

Qt::ItemFlags RGTagModel::flags(const QModelIndex& index) const
{
    return d->tagModel->flags(index);
}

void RGTagModel::slotSourceDataChanged(const QModelIndex& row, const QModelIndex& column)
{
    emit dataChanged(row,column);

}

void RGTagModel::slotSourceHeaderDataChanged(const Qt::Orientation orientation, int first, int last)
{
    emit headerDataChanged(orientation, first, last); 
}

void RGTagModel::slotColumnsAboutToBeInserted ( const QModelIndex & parent, int start, int end )
{
   beginInsertColumns (parent, start, end); 

}
void RGTagModel::slotColumnsAboutToBeMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn )
{
    beginMoveColumns( sourceParent, sourceStart, sourceEnd, destinationParent, destinationColumn );
}
void RGTagModel::slotColumnsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
{
    beginRemoveColumns ( parent, start, end);
}
void RGTagModel::slotColumnsInserted ()
{
    endInsertColumns();
}
void RGTagModel::slotColumnsMoved ()
{
    endMoveColumns();
}
void RGTagModel::slotColumnsRemoved ()
{
    endRemoveColumns();
}
void RGTagModel::slotLayoutAboutToBeChanged ()
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
void RGTagModel::slotRowsAboutToBeInserted (const QModelIndex & parent, int start, int end )
{
    beginInsertRows(parent, start, end);
}
void RGTagModel::slotRowsAboutToBeMoved (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow )
{
    beginMoveRows( sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow );
}
void RGTagModel::slotRowsAboutToBeRemoved (const QModelIndex & parent, int start, int end )
{
    beginRemoveRows(parent, start, end);
}
void RGTagModel::slotRowsInserted ()
{
    endInsertRows();
}
void RGTagModel::slotRowsMoved ()
{
    endMoveRows();
}
void RGTagModel::slotRowsRemoved ()
{
    endRemoveRows();
}


}    //KIPIGPSSyncPlugin
