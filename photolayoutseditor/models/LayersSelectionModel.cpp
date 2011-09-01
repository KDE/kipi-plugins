// Local
#include "LayersSelectionModel.h"
#include "LayersModel.h"

using namespace KIPIPhotoLayoutsEditor;

LayersSelectionModel::LayersSelectionModel(LayersModel * model, QObject *parent) :
    QItemSelectionModel(model, parent)
{
}
