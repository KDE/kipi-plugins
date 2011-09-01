#ifndef ABSTRACTMOVABLEMODEL_H
#define ABSTRACTMOVABLEMODEL_H

#include <QAbstractItemModel>
#include <QDebug>
#include <QModelIndex>
#include <QtGlobal>

namespace KIPIPhotoLayoutsEditor
{
    class MoveRowsCommand;

    class AbstractMovableModel : public QAbstractItemModel
    {
        public:
            AbstractMovableModel(QObject * parent = 0);
            virtual bool moveRows(int sourcePosition, int sourceCount, int destPosition) = 0;
            virtual void setItem(QObject * graphicsItem, const QModelIndex & index) = 0;
            virtual QObject * item(const QModelIndex & index) const = 0;
    };
};

#endif // ABSTRACTMOVABLEMODEL_H
