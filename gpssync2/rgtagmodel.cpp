
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


}    //KIPIGPSSyncPlugin
