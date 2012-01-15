#include "TemplatesModel.h"

using namespace KIPIPhotoLayoutsEditor;

TemplatesModel::TemplatesModel(QObject *parent) :
    QAbstractItemModel(parent)
{
}

QModelIndex TemplatesModel::index(int row, int column, const QModelIndex & parent) const
{
    if (!hasIndex(row, column, parent) || parent.isValid())
        return QModelIndex();

    return createIndex(row, column, templates[row]);
}

int TemplatesModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;
    return 1;
}

int TemplatesModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;
    return templates.count();
}

bool TemplatesModel::insertRows(int row, int count, const QModelIndex & parent)
{
    if (count < 0 || row > this->rowCount())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    while (count--)
        templates.insert(row, 0);
    endInsertRows();
    return true;
}

bool TemplatesModel::removeRows(int row, int count, const QModelIndex & /*parent*/)
{
    while (count--)
        templates[row]->deleteLater();
    return true;
}

QVariant TemplatesModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TemplateItem * item = templates[index.row()];
    if (!item)
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
            return item->name();
        case Qt::DecorationRole:
            return item->icon();
        default:
            return QVariant();
    }
    return QVariant();
}

QModelIndex TemplatesModel::parent(const QModelIndex & child) const
{
    return QModelIndex();
}

void TemplatesModel::addTemplate(const QString & path, const QString & name)
{
    insertRows(rowCount(), 1);
    templates.last() = new TemplateItem(path, name);
}
