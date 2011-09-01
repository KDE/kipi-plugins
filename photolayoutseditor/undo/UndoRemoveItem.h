#ifndef UNDOREMOVEITEM_H
#define UNDOREMOVEITEM_H

#include <QUndoCommand>
#include <QModelIndex>
#include <QGraphicsItem>

namespace KIPIPhotoFramesEditor
{
    class Scene;
    class LayersModel;
    class AbstractPhoto;

    class UndoRemoveItem : public QUndoCommand
    {
            AbstractPhoto * m_item;
            AbstractPhoto * m_parentItem;
            QGraphicsScene * m_scene;
            LayersModel * m_model;
            QModelIndex m_parentIndex;
            QModelIndex m_itemIndex;
            int m_row;

        public:

            UndoRemoveItem(AbstractPhoto * item, Scene * scene, LayersModel * model, QUndoCommand * parent = 0);
            ~UndoRemoveItem();
            virtual void redo();
            virtual void undo();

        private:

            void appendChild(AbstractPhoto * item, const QModelIndex & parent);
            static bool compareGraphicsItems(QGraphicsItem * i1, QGraphicsItem * i2);
    };
}

#endif // UNDOREMOVEITEM_H
