#ifndef LAYERSSELECTIONMODEL_H
#define LAYERSSELECTIONMODEL_H

// Qt
#include <QItemSelectionModel>


namespace KIPIPhotoLayoutsEditor
{
    class LayersModel;

    class LayersSelectionModel : public QItemSelectionModel
    {
            Q_OBJECT

        public:

            explicit LayersSelectionModel(LayersModel * model, QObject *parent = 0);

        signals:

        public slots:

    };
}

#endif // LAYERSSELECTIONMODEL_H
