#include "AbstractMovableModel.h"
#include "global.h"

#include <QUndoCommand>

using namespace KIPIPhotoLayoutsEditor;

AbstractMovableModel::AbstractMovableModel(QObject * parent) :
    QAbstractItemModel(parent)
{
}
